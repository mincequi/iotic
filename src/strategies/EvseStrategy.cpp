#include "EvseStrategy.h"

#include <numeric>

#include <magic_enum.hpp>

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread.hpp>

#include <uvw_iot/common/ThingType.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/Rpp.h>
#include <common/RppUvw.h>
#include <config/Config.h>
#include <things/Site.h>

using namespace uvw_iot::common;

std::unique_ptr<Strategy> EvseStrategy::from(const ThingPtr& thing,
                                             const ThingRepository& repo,
                                             const Site& site,
                                             const Config& cfg) {
    if (thing->type() == ThingType::EvStation) {
        thing->setProperty(ThingPropertyKey::offset, cfg.valueOr<int>(thing->id(), Config::Key::offset, defaultOffset));
        return std::unique_ptr<EvseStrategy>(new EvseStrategy(thing, repo, site, cfg));
    }
    return {};
}

EvseStrategy::EvseStrategy(const ThingPtr& thing,
                           const ThingRepository& repo,
                           const Site& site,
                           const Config& cfg) :
    Strategy(thing->id()),
    _repo(repo),
    _site(site),
    _cfg(cfg) {
    _wantsToSwitch.get_observable()
            | distinct_until_changed()
            | debounce(std::chrono::seconds(cfg.valueOr<int>(thing->id(), Config::Key::debounce, 300)), rpp::schedulers::new_thread())
            | observe_on(rpp_uvw::schedulers::main_thread_scheduler{})
            | subscribe([this](bool wantsToSwitch) {
        if (wantsToSwitch)
            _phases = computePhases();
    });

    thing->propertiesObservable().subscribe([this](const ThingPropertyMap& map) {
        for (const auto& kv : map) {
            switch (kv.first) {
            case ThingPropertyKey::power:
                _power = std::get<int>(kv.second);
                break;
            case ThingPropertyKey::voltage:
                _voltages = std::get<std::array<int, 3>>(kv.second);
                break;
            case ThingPropertyKey::offset:
                _offsetPower = offsetTable[std::get<int>(kv.second)];
                break;
            default:
                break;
            }
        }
    });
}

EvseStrategy::~EvseStrategy() {
}

void EvseStrategy::evaluate(double gridPower) {
    // Compute available power
    const double availablePower = _power - gridPower + _offsetPower;
    if (_shortTermAvailablePower == 0.0) _shortTermAvailablePower = availablePower;
    if (_longTermAvailablePower == 0.0) _longTermAvailablePower = availablePower;
    _shortTermAvailablePower += _cfg.evseAlpha() * (availablePower - _shortTermAvailablePower);
    _longTermAvailablePower += _cfg.evseBeta() * (availablePower - _longTermAvailablePower);

    // Compute possible phases
    const Phases phases = computePhases();
    if (!_phases.has_value()) _phases = phases;

    // If current phases differ, trigger switch subject
    _wantsToSwitch.get_observer().on_next(_phases.value() != phases);

    LOG_S(1) << thingId() <<
                "> longTermAvailablePower: " << _longTermAvailablePower <<
                ", shortTermAvailablePower: " << _shortTermAvailablePower;

    // Set current
    _repo.setThingProperty(thingId(), ThingPropertyKey::current, computeCurrent());
}

EvseStrategy::Phases EvseStrategy::computePhases() const {
    if (_longTermAvailablePower > (6 * std::reduce(_voltages.begin(), _voltages.end()))) {
        return Phases::three_phase;
    } else if (_longTermAvailablePower > (6 * _voltages.front())) {
        return Phases::single_phase;
    }
    return Phases::off;
}

ThingPropertyValue EvseStrategy::computeCurrent() const {
    switch (_phases.value()) {
    case Phases::single_phase:
        return (int)(_shortTermAvailablePower / _voltages.front());
    case Phases::three_phase: {
        const int amps = _shortTermAvailablePower / std::reduce(_voltages.begin(), _voltages.end());
        return std::array{ amps, amps, amps };
    }
    default:
        return 0;
    }
}
