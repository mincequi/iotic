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
#include <things/Site.h>
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
    _wantsToSwitch.get_observable()
            .distinct_until_changed()
            .debounce(std::chrono::milliseconds(cfg->valueOr<int>(thing->id(), Config::Key::debounce, 300000)), rpp::schedulers::new_thread())
            .observe_on(rppqt::schedulers::main_thread_scheduler{})
            .subscribe([this](bool wantsToSwitch) {
        if (wantsToSwitch)
            _phases = computePhases();
    });

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

    _site->properties().subscribe([this](const std::map<Property, Value>& map) {
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
    // If _gridPower is not set yet, do nothing
    if (_gridPower == 0.0) return;

    // Compute available power
    const double availablePower = _power - _gridPower + _offsetPower;
    if (_shortTermAvailablePower == 0.0) _shortTermAvailablePower = availablePower;
    if (_longTermAvailablePower == 0.0) _longTermAvailablePower = availablePower;
    _shortTermAvailablePower += cfg->evseAlpha() * (availablePower - _shortTermAvailablePower);
    _longTermAvailablePower += cfg->evseBeta() * (availablePower - _longTermAvailablePower);

    // Compute possible phases
    const Phases phases = computePhases();
    if (!_phases.has_value()) _phases = phases;

    // If current phases differ, trigger switch subject
    _wantsToSwitch.get_subscriber().on_next(_phases.value() != phases);

    LOG_S(1) << thingId() <<
                "> longTermAvailablePower: " << _longTermAvailablePower <<
                ", shortTermAvailablePower: " << _shortTermAvailablePower;

    // Set current
    repo->setThingProperty(thingId(), MutableProperty::current, computeCurrent());
}

EvseStrategy::Phases EvseStrategy::computePhases() const {
    if (_longTermAvailablePower > (6 * std::reduce(_voltages.begin(), _voltages.end()))) {
        return Phases::three_phase;
    } else if (_longTermAvailablePower > (6 * _voltages.front())) {
        return Phases::single_phase;
    }
    return Phases::off;
}

Value EvseStrategy::computeCurrent() const {
    switch (_phases.value()) {
    case Phases::single_phase:
        return _shortTermAvailablePower / _voltages.front();
    case Phases::three_phase: {
        const double amps = _shortTermAvailablePower / std::reduce(_voltages.begin(), _voltages.end());
        return std::array{ amps, amps, amps };
    }
    default:
        return 0.0;
    }
}
