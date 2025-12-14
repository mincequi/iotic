#include "ThreePhaseStrategy.h"

#include <uvw_iot/ThingType.h>

#include <config/ConfigRepository.h>

using namespace uvw_iot;

std::unique_ptr<Strategy> ThreePhaseStrategy::from(
    const ThingPtr& thing,
    const ConfigRepository& configRepository) {
    if (thing->type() == ThingType::Relay) {
        auto powerThresholds = configRepository.valueOr<std::vector<int>>(thing->id(), ConfigRepository::Key::power_thresholds);
        if (powerThresholds.empty()) {
            return {};
        }
        return std::unique_ptr<ThreePhaseStrategy>(new ThreePhaseStrategy(thing, powerThresholds));
    }
    return {};
}

ThreePhaseStrategy::ThreePhaseStrategy(
    const ThingPtr& thing,
    const std::vector<int>& powerThresholds) :
    Strategy(thing->id()),
    _powerThresholds(powerThresholds),
    _hystereses(powerThresholds.size()) {

    std::transform(_powerThresholds.begin(), _powerThresholds.end(), _hystereses.begin(), [](int x) {
        return x;
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
    //return siteProperties.measuredPower < _powerThresholds[std::max(0, currentStep() - 1)];
    return false;
}
