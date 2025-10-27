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
                                             const ConfigRepository& cfg) {
    if (thing->type() == ThingType::EvStation) {
        thing->setProperty(ThingPropertyKey::offset, cfg.valueOr<int>(thing->id(), ConfigRepository::Key::offset, defaultOffset));
        return std::unique_ptr<EvseStrategy>(new EvseStrategy(thing, repo, cfg));
    }
    return {};
}

EvseStrategy::EvseStrategy(const ThingPtr& thing,
                           const ThingRepository& repo,
                           const ConfigRepository& cfg) :
    Strategy(thing->id()),
    _thingRepository(repo),
    _configRepository(cfg) {

    if (thing->properties().contains(ThingPropertyKey::offset)) {
        _offsetPower = offsetTable[std::get<int>(thing->properties().at(ThingPropertyKey::offset))];
    }

    thing->propertiesObservable().subscribe([this](const ThingPropertyMap& map) {
        for (const auto& kv : map) {
            switch (kv.first) {
            case ThingPropertyKey::power:
                _measuredPower = std::get<int>(kv.second);
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

bool EvseStrategy::wantsToTurnOff(const Site::Properties& siteProperties) {
    auto longTermAvailablePower = _offsetPower + _measuredPower - siteProperties.longTermGridPower;
    _nextPhases = computePhases(longTermAvailablePower);
    const bool wantsToSwitchPhases = _nextPhases < _phases;
    LOG_IF_S(INFO, wantsToSwitchPhases) << this->thingId() << "> offsetPower: " << _offsetPower << ", power: " << _measuredPower << ", longTermGridPower: " << siteProperties.longTermGridPower;
    actuate(siteProperties);

    return wantsToSwitchPhases;
}

bool EvseStrategy::wantsToTurnOn(const Site::Properties& siteProperties) {
    auto longTermAvailablePower = _offsetPower + _measuredPower - siteProperties.longTermGridPower;
    _nextPhases = computePhases(longTermAvailablePower);
    const bool wantsToSwitchPhases = _nextPhases > _phases;
    LOG_IF_S(INFO, wantsToSwitchPhases) << this->thingId() << "> offsetPower: " << _offsetPower << ", power: " << _measuredPower << ", longTermGridPower: " << siteProperties.longTermGridPower;
    actuate(siteProperties);

    return wantsToSwitchPhases;
}

void EvseStrategy::actuate(const Site::Properties& siteProperties) {
    const bool wantsToSwitchPhases = _phases != _nextPhases;
    const auto now = std::chrono::system_clock::now();
    if (wantsToSwitchPhases && (_lastActuationTs + std::chrono::seconds(180) < now)) {
        _lastActuationTs = now;
        LOG_S(INFO) << this->thingId() << "> phases: " << _phases << " to " << _nextPhases;
        _phases = _nextPhases;
    }

    const std::chrono::duration remainingBeforeSwitch = _lastActuationTs + std::chrono::seconds(180) - std::chrono::system_clock::now();
    if (wantsToSwitchPhases && remainingBeforeSwitch > 0s) {
        _thingRepository.setThingProperties(thingId(), {
            { ThingPropertyKey::phases, _phases },
            { ThingPropertyKey::next_phases, _nextPhases },
            { ThingPropertyKey::countdown, (int)std::chrono::duration_cast<std::chrono::seconds>(remainingBeforeSwitch).count() }
        });
    } else {
        _thingRepository.setThingProperties(thingId(), {
            { ThingPropertyKey::phases, _phases },
            { ThingPropertyKey::next_phases, _nextPhases },
            { ThingPropertyKey::countdown, 0 }
        });
    }

    // Set current
    const auto shortTermAvailablePower = _offsetPower + _measuredPower - siteProperties.shortTermGridPower;
    _thingRepository.setThingProperties(thingId(), {{ ThingPropertyKey::current, computeCurrent(shortTermAvailablePower) }});
}

int EvseStrategy::powerError() const {
    if (_phases == 0) {
        return 0;
    } else if (_phases == 1) {
        return _current * _voltages.front() - _measuredPower;
    } else {
        return _current * std::reduce(_voltages.begin(), _voltages.end()) - _measuredPower;
    }
}

json EvseStrategy::toJson() const {
    json j;
    j["thingId"] = thingId();
    j["type"] = "EvseStrategy";
    j["priority"] = priority();
    j["phases"] = _phases;
    j["nextPhases"] = _nextPhases;
    j["power"] = _measuredPower;
    j["voltages"] = _voltages;
    j["offsetPower"] = _offsetPower;
    j["powerError"] = powerError();
    return j;
}

int EvseStrategy::computePhases(double availablePower) const {
    if (availablePower > (6 * std::reduce(_voltages.begin(), _voltages.end()))) {
        return 3;
    } else if (availablePower > (6 * _voltages.front())) {
        return 1;
    } else {
        return 0;
    }
}

ThingPropertyValue EvseStrategy::computeCurrent(double availablePower) {
    switch (_phases) {
    case 1:
        _current = (int)(availablePower / _voltages.front());
        return _current;
    case 3: {
        _current = availablePower / std::reduce(_voltages.begin(), _voltages.end());
        return std::array{ _current, _current, _current };
    }
    default:
        _current = 0;
        return 0;
    }
}
