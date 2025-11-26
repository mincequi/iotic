#include "EvseStrategy.h"

#include <numeric>

#include <magic_enum.hpp>

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>

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
        ThingPropertyMap properties;
        properties.set<ThingPropertyKey::offset>(cfg.valueOr<int>(thing->id(), ConfigRepository::Key::offset, defaultOffset));
        thing->setProperties(properties);
        return std::unique_ptr<EvseStrategy>(new EvseStrategy(thing, repo, cfg));
    }
    return {};
}

EvseStrategy::EvseStrategy(const ThingPtr& thing,
                           const ThingRepository& repo,
                           const ConfigRepository& cfg) :
    Strategy(thing->id()),
    _thingRepository(repo),
    _configRepository(cfg),
    _onePhaseHysteresis(_configRepository.hysteresisFor(6 * 230)),
    _threePhaseHysteresis(_configRepository.hysteresisFor(3 * 6 * 230)) {

    thing->properties().on<ThingPropertyKey::offset>([this](const auto& value) {
        _offsetPower = offsetTable[value];
    });

    thing->propertiesObservable().subscribe([this](const ThingPropertyMap& map) {
        const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

        map.on<ThingPropertyKey::power>([&](const auto& value) {
            ema(_measuredPower, std::max(value, 0), nowMs - _lastMeasurementTs, _configRepository.shortTermTau);
        }).on<ThingPropertyKey::voltage>([&](const auto& value) {
            _voltages = value;
        }).on<ThingPropertyKey::offset>([&](const auto& value) {
            _offsetPower = offsetTable[value];
        }).on<ThingPropertyKey::status>([this](const auto& value) {
            // make ThingStatus from int
            onStatus(static_cast<ThingStatus>(value));
        });

        _lastMeasurementTs = nowMs;
    });
}

EvseStrategy::~EvseStrategy() {
}

bool EvseStrategy::wantsToStepDown(const Site::Properties& siteProperties) const {
    auto longTermAvailablePower = std::max(0, _offsetPower) + _measuredPower - siteProperties.longTermGridPower;
    _nextPhases = computePhases(longTermAvailablePower);
    return _nextPhases < _phases;
}

bool EvseStrategy::wantsToStepUp(const Site::Properties& siteProperties) const {
    auto longTermAvailablePower = _offsetPower + _measuredPower - siteProperties.longTermGridPower;
    _nextPhases = computePhases(longTermAvailablePower, true);
    if (_nextPhases > _phases) {
        LOG_S(INFO) << this->thingId() << "> offsetPower: " << _offsetPower
                    << ", measuredPower: " << _measuredPower
                    << ", longTermGridPower: " << siteProperties.longTermGridPower;
        return true;
    }
    return false;
}

void EvseStrategy::adjust(Step step, const Site::Properties& siteProperties) {
    if (step != Step::Keep) {
        LOG_S(INFO) << this->thingId() << "> switch phases from: " << _phases << " to " << _nextPhases;
        _phases = _nextPhases;
    }

    ThingPropertyMap properties;
    properties.set<ThingPropertyKey::phases>(_phases);
    properties.set<ThingPropertyKey::next_phases>(_nextPhases);
    properties.set<ThingPropertyKey::countdown>(0);

    // Set current
    const auto shortTermAvailablePower = std::max(0, _offsetPower) + _measuredPower - siteProperties.shortTermGridPower;
    properties.set<ThingPropertyKey::current>(computeCurrent(shortTermAvailablePower));

    _thingRepository.setThingProperties(thingId(), properties);
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
    j["measuredPower"] = _measuredPower;
    j["voltages"] = _voltages;
    j["offsetPower"] = _offsetPower;
    j["powerError"] = powerError();
    j["lastMeasurementTs"] = _lastMeasurementTs.count();
    j["onePhaseHysteresis"] = _onePhaseHysteresis;
    j["threePhaseHysteresis"] = _threePhaseHysteresis;
    j["status"] = magic_enum::enum_name(_status);
    return j;
}

int EvseStrategy::computePhases(double availablePower, bool applyHysteresis) const {
    const auto onePhaseHysteresis = applyHysteresis ? _onePhaseHysteresis : 0;
    const auto threePhaseHysteresis = applyHysteresis ? _threePhaseHysteresis : 0;

    if ((availablePower - threePhaseHysteresis) > (6 * std::reduce(_voltages.begin(), _voltages.end()))) {
        return 3;
    } else if ((availablePower - onePhaseHysteresis) > (6 * _voltages.front())) {
        return 1;
    } else {
        return 0;
    }
}

int EvseStrategy::computeCurrent(double availablePower) {
    switch (_phases) {
    case 1:
        _current = (int)(availablePower / _voltages.front());
        return _current;
    case 3: {
        _current = availablePower / std::reduce(_voltages.begin(), _voltages.end());
        return _current;
    }
    default:
        _current = 0;
        return 0;
    }
}

void EvseStrategy::onStatus(ThingStatus status) {
    _status = status;
}
