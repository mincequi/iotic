#include "MultiPhaseStrategy.h"

#include <algorithm>

#include <uvw_iot/ThingType.h>

#include <common/Logger.h>
#include <config/ConfigRepository.h>

using namespace uvw_iot;

std::unique_ptr<Strategy> MultiPhaseStrategy::from(const ThingPtr& thing,
                                                   const ConfigRepository& configRepository) {
    if (thing->type() != ThingType::Relay) {
        return {};
    }

    auto powerThresholds = configRepository.valueOr<std::vector<int>>(thing->id(), ConfigRepository::Key::power_thresholds);
    if (powerThresholds.size() == 0) {
        return {};
    }

    return std::unique_ptr<MultiPhaseStrategy>(new MultiPhaseStrategy(thing, configRepository, powerThresholds));
}

MultiPhaseStrategy::MultiPhaseStrategy(
    const ThingPtr& thing,
    const ConfigRepository& configRepositor,
    const std::vector<int>& powerThresholds) :
    Strategy(thing->id()),
    _thing(thing),
    _configRepository(configRepositor),
    _powerThresholds(powerThresholds),
    _hystereses(powerThresholds.size()),
    _currentStates(powerThresholds.size(), false),
    _timestamps(powerThresholds.size(), 0),
    _allowedPhaseCount(powerThresholds.size()) {
    std::transform(_powerThresholds.begin(), _powerThresholds.end(), _hystereses.begin(), [this](int x) {
        return _configRepository.hysteresisFor(x);
    });
}

MultiPhaseStrategy::~MultiPhaseStrategy() {
}

json MultiPhaseStrategy::toJson() const {
    json j;
    j["thingId"] = thingId();
    j["type"] = "ThreePhaseStrategy";
    j["priority"] = priority();
    j["powerThresholds"] = _powerThresholds;

    return j;
}

bool MultiPhaseStrategy::wantsToStepDown(const Site::Properties& siteProperties) const {
    const auto [inputs, states] = inputsAndStates();
    if (inputs.size() != _powerThresholds.size() || states.size() != _powerThresholds.size()) {
        return false;
    }

    // If next phase is not active, cannot step down
    if (!states[_nextStepDownPhase]) {
        return false;
    }

    // If next phase input is not active, we want to step down
    if (!inputs[_nextStepDownPhase]) {
        decrementPhaseCount(siteProperties);
        return true;
    }

    if (siteProperties.longTermGridPower > 0) {
        return true;
    }

    return false;
}

bool MultiPhaseStrategy::wantsToStepUp(const Site::Properties& siteProperties) const {
    const auto [inputs, states] = inputsAndStates();
    if (inputs.size() != _powerThresholds.size() || states.size() != _powerThresholds.size()) {
        return false;
    }

    // If next phase is already active, cannot step up
    if (states[_nextStepUpPhase]) {
        return false;
    }

    // If next phase input is not active, we won't step up
    if (!inputs[_nextStepUpPhase]) {
        return false;
    }

    // If we are currently limited due to temperature, we won't step up
    incrementPhaseCount(siteProperties);
    const auto phaseCount = std::count(states.begin(), states.end(), true);
    if (phaseCount >= _allowedPhaseCount) {
        return false;
    }

    if (siteProperties.longTermGridPower < (-_powerThresholds[_nextStepUpPhase] - _hystereses[_nextStepUpPhase])) {
        return true;
    }

    return false;
}

void MultiPhaseStrategy::adjust(Step step, const Site::Properties& siteProperties) {
    int day = siteProperties.ts / (24 * 3600);
    if (day != _currentDay) {
        _currentDay = day;
        _energyDelivered = 0;
    }

    switch (step) {
    case Step::Keep:
        break;
    case Step::Down:
        _energyDelivered += (siteProperties.ts - _timestamps[_nextStepDownPhase]) * _powerThresholds[_nextStepDownPhase];
        _currentStates[_nextStepDownPhase] = false;
        _nextStepDownPhase = (_nextStepDownPhase + 1) % _currentStates.size();
        break;
    case Step::Up:
        _timestamps[_nextStepUpPhase] = siteProperties.ts;
        _currentStates[_nextStepUpPhase] = true;
        _nextStepUpPhase = (_nextStepUpPhase + 1) % _currentStates.size();
        break;
    }

    // Sum up energy delivered of current active phases
    int energyCurrent = 0;
    for (size_t i = 0; i < _currentStates.size(); ++i) {
        if (_currentStates[i]) {
            energyCurrent += (siteProperties.ts - _timestamps[i]) * _powerThresholds[i];
        }
    }

    ThingPropertyMap properties;
    properties.set<ThingPropertyKey::multistateSelector>(_currentStates);
    // Convert Ws into kWm
    properties.set<ThingPropertyKey::energy>((_energyDelivered + energyCurrent) / 60000);
    _thing->setProperties(properties);

    if (step != Step::Keep) {
        std::stringstream ss;
        ss << "[ ";
        for (const auto state : _currentStates) {
            ss << (state ? "1 " : "0 ");
        }
        ss << "]";

        LOG_S(INFO) << this->thingId() << "> set phases: " << ss.str();
    }
}

std::pair<std::vector<bool>, std::vector<bool>> MultiPhaseStrategy::inputsAndStates() const {
    std::vector<bool> digitalInputs;
    std::vector<bool> multistateSelector;

    _thing->properties()
        .on<ThingPropertyKey::digitalInput>([&](const auto& value) { digitalInputs = value; })
        .on<ThingPropertyKey::multistateSelector>([&](const auto& value) { multistateSelector = value; });

    return { digitalInputs, multistateSelector };
}

void MultiPhaseStrategy::decrementPhaseCount(const Site::Properties& siteProperties) const {
    if (_allowedPhaseCount == 0) {
        return;
    }

    // Do not decrement phases multiple times within 30 minutes
    if (siteProperties.ts - _lastDecrementPhaseCountTimestamp < 1800) {
        return;
    }

    // Last decrement is more than 30 minutes ago and input is still not activ, we decrement one more phase
    _lastDecrementPhaseCountTimestamp = siteProperties.ts;
    _allowedPhaseCount = std::max(_allowedPhaseCount - 1, 0);
    LOG_S(INFO) << "decrement phase count to " << _allowedPhaseCount;
}

void MultiPhaseStrategy::incrementPhaseCount(const Site::Properties& siteProperties) const {
    if (_allowedPhaseCount >= (int)_powerThresholds.size()) {
        return;
    }

    // If we increased phases in the last 30 minutes, we won't increase again to avoid too frequent switching due to temperature limits
    if (siteProperties.ts - _lastDecrementPhaseCountTimestamp < 1800) {
        return;
    }

    _allowedPhaseCount = std::min(_allowedPhaseCount + 1, (int)_powerThresholds.size());
    LOG_S(INFO) << "increase phase count to " << _allowedPhaseCount;
}
