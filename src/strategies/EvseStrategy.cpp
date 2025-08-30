#include "EvseStrategy.h"

#include <numeric>

#include <magic_enum.hpp>

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>

#include <uvw_iot/ThingType.h>
#include <uvw_iot/util/Filter.h>
#include <uvw_iot/util/Site.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/Rpp.h>
#include <common/RppUvw.h>
#include <config/ConfigRepository.h>

using namespace uvw_iot;
using namespace uvw_iot::util;

std::unique_ptr<Strategy> EvseStrategy::from(const ThingPtr& thing,
                                             const ThingRepository& repo,
                                             const Site& site,
                                             const ConfigRepository& cfg) {
    if (thing->type() == ThingType::EvStation) {
        thing->setProperty(ThingPropertyKey::offset, cfg.valueOr<int>(thing->id(), ConfigRepository::Key::offset, defaultOffset));
        return std::unique_ptr<EvseStrategy>(new EvseStrategy(thing, repo, site, cfg));
    }
    return {};
}

EvseStrategy::EvseStrategy(const ThingPtr& thing,
                           const ThingRepository& repo,
                           const Site& site,
                           const ConfigRepository& cfg) :
    Strategy(thing->id()),
    _repo(repo),
    _site(site),
    _cfg(cfg) {
    _doPhaseSwitch.get_observable()
        | distinct_until_changed()
        | map([this](bool doSwitch) {
              _nextSwitch = std::chrono::system_clock::now() + std::chrono::seconds(_cfg.valueOr<int>(thingId(), ConfigRepository::Key::debounce, 180));

              return doSwitch;
          })
        | debounce(std::chrono::seconds(cfg.valueOr<int>(thing->id(), ConfigRepository::Key::debounce, 180)), rpp_uvw::schedulers::main_thread_scheduler{})
        | subscribe([this](bool doSwitch) {
              if (doSwitch) {
                  _phases = _nextPhases;
              }
          });

    if (thing->properties().contains(ThingPropertyKey::offset)) {
        _offsetPower = offsetTable[std::get<int>(thing->properties().at(ThingPropertyKey::offset))];
    }
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

void EvseStrategy::evaluate(const Site::Properties& siteProperties) const {
    // Compute available power
    const double availablePower = _offsetPower + _power - siteProperties.gridPower;
    if (_shortTermAvailablePower <= 0) _shortTermAvailablePower = availablePower;
    if (_longTermAvailablePower <= 0) _longTermAvailablePower = availablePower;
    ema(_shortTermAvailablePower, availablePower, std::chrono::milliseconds((siteProperties.ts - _prevTs) * 1000), 10000ms);
    ema(_longTermAvailablePower, availablePower, std::chrono::milliseconds((siteProperties.ts - _prevTs) * 1000),  40000ms);
    _prevTs = siteProperties.ts;

    // Compute possible phases
    const auto phases = computePhases(_longTermAvailablePower);

    // If current phases differ, trigger switch subject
    _doPhaseSwitch.get_observer().on_next(_phases != phases);

    const std::chrono::duration remainingBeforeSwitch = _nextSwitch - std::chrono::system_clock::now();
    if (_nextPhases != _phases && remainingBeforeSwitch > 0s) {
        _repo.setThingProperties(thingId(), {
            { ThingPropertyKey::phases, _phases },
            { ThingPropertyKey::next_phases, _nextPhases },
            { ThingPropertyKey::countdown, (int)std::chrono::duration_cast<std::chrono::seconds>(remainingBeforeSwitch).count() }
        });
    } else {
        _repo.setThingProperties(thingId(), {
            { ThingPropertyKey::phases, _phases },
            { ThingPropertyKey::next_phases, _nextPhases },
            { ThingPropertyKey::countdown, 0 }
        });
    }

    // Set current
    _repo.setThingProperty(thingId(), ThingPropertyKey::current, computeCurrent(_shortTermAvailablePower));
}

int EvseStrategy::computePhases(double availablePower) const {
    if (availablePower > (6 * std::reduce(_voltages.begin(), _voltages.end()))) {
        _nextPhases = 3;
    } else if (availablePower > (6 * _voltages.front())) {
        _nextPhases = 1;
    } else {
        _nextPhases = 0;
    }

    return _nextPhases;
}

ThingPropertyValue EvseStrategy::computeCurrent(double availablePower) const {
    switch (_phases) {
    case 1:
        return (int)(availablePower / _voltages.front());
    case 3: {
        const int amps = availablePower / std::reduce(_voltages.begin(), _voltages.end());
        return std::array{ amps, amps, amps };
    }
    default:
        return 0;
    }
}
