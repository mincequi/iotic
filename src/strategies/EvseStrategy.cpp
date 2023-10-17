#include "EvseStrategy.h"

#include <numeric>

#include <magic_enum.hpp>

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

std::unique_ptr<Strategy> EvseStrategy::from(const ThingPtr& thing) {
    if (thing->type() == Thing::Type::EvStation) {
        thing->setProperty(MutableProperty::offset, 4.0);
        return std::unique_ptr<EvseStrategy>(new EvseStrategy(thing));
    }
    return {};
}

EvseStrategy::EvseStrategy(const ThingPtr& thing) :
    Strategy(thing->id()) {
    _phasesSubject.get_observable()
            .distinct_until_changed()
            .debounce(std::chrono::milliseconds(cfg->valueOr<int>(thing->id(), Config::Key::debounce, 300000)), rpp::schedulers::new_thread())
            .observe_on(rppqt::schedulers::main_thread_scheduler{})
            .subscribe([this](const Phases& phases) {
        _phasesState = phases;
    });
    //_isOnSubject.get_observable()
    //        .distinct_until_changed()
    //        .debounce(std::chrono::milliseconds(cfg->valueOr<int>(thing->id(), Config::Key::debounce, 300000)), rpp::schedulers::new_thread())
    //        .observe_on(rppqt::schedulers::main_thread_scheduler{})
    //        .subscribe([this](bool isOn) {
    //    _isOnState = isOn;
    //});

    thing->properties().subscribe([this](const std::map<Property, Value>& map) {
        for (const auto& kv : map) {
            switch (kv.first) {
            case Property::power:
                _power = std::get<double>(kv.second);
                break;
            case Property::voltage:
                _voltages = std::get<std::array<double, 3>>(kv.second);
                break;
            case Property::offset:
                _offsetPower = offsetTable[std::get<double>(kv.second)];
                break;
            default:
                break;
            }
        }
    });

    repo->site().properties().subscribe([this](const std::map<Property, Value>& map) {
        for (const auto& kv : map) {
            switch (kv.first) {
            case Property::grid_power:
                _gridPower = std::get<double>(kv.second);
                break;
            default:
                break;
            }
        }
    });
}

EvseStrategy::~EvseStrategy() {
}

void EvseStrategy::evaluate() {
    // if _gridPower is not set yet.
    if (_gridPower == 0.0) return;

    const double availablePower = _power - _gridPower + _offsetPower;
    // Init _availablePower
    if (_availablePower == 0.0) _availablePower = availablePower;

    _availablePower = (_availablePower + availablePower) / 2.0;
    const double sumVoltage = std::reduce(_voltages.begin(), _voltages.end());
    Phases phases = Phases::off;
    bool isOn = false;
    if (_availablePower > (6 * sumVoltage)) {
        phases = Phases::three_phase;
        isOn = true;
    } else if (_availablePower > (6 * _voltages.front())) {
        phases = Phases::single_phase;
        isOn = true;
    }

    // Set initial values
    if (!_phasesState.has_value()) _phasesState = phases;
    //if (!_isOnState.has_value()) _isOnState = isOn;

    // Trigger subjects
    _phasesSubject.get_subscriber().on_next(phases);
    //_isOnSubject.get_subscriber().on_next(isOn);

    // Emit
    double amps = 0;
    switch (_phasesState.value()) {
    case Phases::single_phase:
        amps = _availablePower / _voltages.front();
        repo->setThingProperty(thingId(), MutableProperty::current, amps);
        break;
    case Phases::three_phase:
        amps = _availablePower / sumVoltage;
        repo->setThingProperty(thingId(), MutableProperty::current, std::array{ amps, amps, amps });
        break;
    default:
        repo->setThingProperty(thingId(), MutableProperty::current, 0.0);
        break;
    }
    LOG_S(INFO) << this->thingId() << "> { \"phases\": " << phases << ", \"amps\": " << amps << " }";
    //repo->setThingProperty(thingId(), MutableProperty::actuation, _isOnState.value());
    //LOG_S(INFO) << this->thingId() << "> { \"isOn\": " << _isOnState.value() << ", \"phases\": " << _phasesState.value() << ", \"amps\": " << amps << " }";
}
