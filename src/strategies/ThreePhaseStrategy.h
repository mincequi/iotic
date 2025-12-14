#pragma once

#include "Strategy.h"

class ConfigRepository;

class ThreePhaseStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(
        const ThingPtr& thing,
        const ConfigRepository& configRepository
    );
    ~ThreePhaseStrategy();

private:
    ThreePhaseStrategy(
        const ThingPtr& thing,
        const std::vector<int>& powerThresholds
    );

    json toJson() const override;
    bool wantsToStepDown(const Site::Properties& siteProperties) const override;
    bool wantsToStepUp(const Site::Properties& siteProperties) const override;
    void adjust(Step step, const Site::Properties& siteProperties) override;

    std::vector<int> _powerThresholds;
    std::vector<int> _hystereses;
};
