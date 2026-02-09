#pragma once

#include <memory>

#include "Strategy.h"

class ConfigRepository;

class MultiPhaseStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ConfigRepository& configRepository);
    ~MultiPhaseStrategy();

    bool wantsToStepDown(const Site::Properties& siteProperties) const override;
    bool wantsToStepUp(const Site::Properties& siteProperties) const override;
    void adjust(Step step, const Site::Properties& siteProperties) override;

private:
    MultiPhaseStrategy(
        const ThingPtr& thing,
        const ConfigRepository& configRepository,
        const std::vector<int>& powerThresholds
    );

    json toJson() const override;

    // return digital inputs and multistate selector values
    std::pair<std::vector<bool>, std::vector<bool>> inputsAndStates() const;

    // temp
    void decrementPhaseCount(const Site::Properties& siteProperties) const;
    void incrementPhaseCount(const Site::Properties& siteProperties) const;
    mutable int _lastDecrementPhaseCountTimestamp = 0;
    mutable int _allowedPhaseCount = 0;

    const ThingPtr _thing;
    const ConfigRepository& _configRepository;
    std::vector<int> _powerThresholds;
    std::vector<int> _hystereses;
    int _nextStepUpPhase = 0;
    int _nextStepDownPhase = 0;
    std::vector<bool> _currentStates;
    std::vector<int> _timestamps;
    int _energyDelivered = 0; // Ws
    int _currentDay = 0;
};
