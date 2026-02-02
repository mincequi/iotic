#include "ThreePhaseStrategy.h"

#include <uvw_iot/ThingType.h>

#include <common/Logger.h>
#include <config/ConfigRepository.h>

using namespace uvw_iot;

std::unique_ptr<Strategy> ThreePhaseStrategy::from(const ThingPtr& thing,
                                                   const ConfigRepository& configRepository) {
    if (thing->type() != ThingType::Relay) {
        return {};
    }

    auto powerThresholds = configRepository.valueOr<std::vector<int>>(thing->id(), ConfigRepository::Key::power_thresholds);
    if (powerThresholds.size() == 0) {
        return {};
    }

    return std::unique_ptr<ThreePhaseStrategy>(new ThreePhaseStrategy(thing, configRepository, powerThresholds));
}

ThreePhaseStrategy::ThreePhaseStrategy(
    const ThingPtr& thing,
    const ConfigRepository& configRepositor,
    const std::vector<int>& powerThresholds) :
    Strategy(thing->id()),
    _thing(thing),
    _configRepository(configRepositor),
    _powerThresholds(powerThresholds),
    _hystereses(powerThresholds.size()),
    _lastStates(powerThresholds.size(), false) {

    std::transform(_powerThresholds.begin(), _powerThresholds.end(), _hystereses.begin(), [this](int x) {
        return _configRepository.hysteresisFor(x);
    });
}

ThreePhaseStrategy::~ThreePhaseStrategy() {
}

json ThreePhaseStrategy::toJson() const {
    json j;
    j["thingId"] = thingId();
    j["type"] = "ThreePhaseStrategy";
    j["priority"] = priority();
    j["powerThresholds"] = _powerThresholds;

    return j;
}

bool ThreePhaseStrategy::wantsToStepDown(const Site::Properties& siteProperties) const {
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
        return true;
    }

    if (siteProperties.longTermGridPower > 0) {
        return true;
    }

    return false;
}

bool ThreePhaseStrategy::wantsToStepUp(const Site::Properties& siteProperties) const {
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

    if (siteProperties.longTermGridPower < (-_powerThresholds[_nextStepUpPhase] - _hystereses[_nextStepUpPhase])) {
        return true;
    }

    return false;
}

void ThreePhaseStrategy::adjust(Step step, const Site::Properties& siteProperties) {
    switch (step) {
    case Step::Keep:
        break;
    case Step::Down:
        _lastStates[_nextStepDownPhase] = false;
        _nextStepDownPhase = (_nextStepDownPhase + 1) % _lastStates.size();
        break;
    case Step::Up:
        _lastStates[_nextStepUpPhase] = true;
        _nextStepUpPhase = (_nextStepUpPhase + 1) % _lastStates.size();
        break;
    }

    ThingPropertyMap properties;
    properties.set<ThingPropertyKey::multistateSelector>(_lastStates);
    _thing->setProperties(properties);

    if (step != Step::Keep) {
        std::stringstream ss;
        ss << "[ ";
        for (const auto state : _lastStates) {
            ss << (state ? "1 " : "0 ");
        }
        ss << "]";

        LOG_S(INFO) << this->thingId() << "> set phases: " << ss.str();
    }
}

std::pair<std::vector<bool>, std::vector<bool>> ThreePhaseStrategy::inputsAndStates() const {
    std::vector<bool> digitalInputs;
    std::vector<bool> multistateSelector;

    _thing->properties()
        .on<ThingPropertyKey::digitalInput>([&](const auto& value) { digitalInputs = value; })
        .on<ThingPropertyKey::multistateSelector>([&](const auto& value) { multistateSelector = value; });

    return { digitalInputs, multistateSelector };
}

