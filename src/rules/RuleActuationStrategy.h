#pragma once

#include <memory>
#include <optional>

#include <uvw_iot/ThingRepository.h>

#include <common/Rpp.h>
#include <strategies/Strategy.h>

using uvw_iot::ThingPtr;
using uvw_iot::ThingRepository;

class te_parser;
class ConfigRepository;
class RuleEngine;
class SymbolRepository;

class RuleActuationStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const SymbolRepository& symbolRepository,
                                          const RuleEngine& rules,
                                          const ConfigRepository& cfg);

private:
    RuleActuationStrategy(const ThingPtr& thing,
                          std::unique_ptr<te_parser> onExpression,
                          std::unique_ptr<te_parser> offExpression,
                          const ThingRepository& repo);

    json toJson() const override;

    bool wantsToStepDown(const Site::Properties& siteProperties) const override;
    bool wantsToStepUp(const Site::Properties& siteProperties) const override;
    void adjust(Step step, const Site::Properties& siteProperties) override;

    const ThingRepository& _thingRepository;

    std::unique_ptr<te_parser> _onExpression;
    std::unique_ptr<te_parser> _offExpression;
    mutable std::optional<bool> _state;
    mutable std::optional<bool> _nextState;

    int _measuredPower = 0;
};
