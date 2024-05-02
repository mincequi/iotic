#pragma once

#include <memory>
#include <optional>

#include <uvw_iot/ThingRepository.h>

#include <common/Rpp.h>
#include <strategies/Strategy.h>

using uvw_iot::ThingPtr;
using uvw_iot::ThingRepository;

class te_parser;
class Config;
class RulesEngine;

template<rpp::schedulers::constraint::scheduler TScheduler>
class RuleActuationStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const RulesEngine& rules,
                                          const Config& cfg);

    RuleActuationStrategy(const std::string& thingId,
                          std::unique_ptr<te_parser> onExpression,
                          std::unique_ptr<te_parser> offExpression,
                          const ThingRepository& repo,
                          const Config& cfg);

    void evaluate(double gridPower) override;

private:
    const ThingRepository& _repo;
    const Config& _cfg;

    std::unique_ptr<te_parser> _onExpression;
    std::unique_ptr<te_parser> _offExpression;
    TScheduler _scheduler;
    publish_subject<bool> _expressionSubject;
    std::optional<bool> _actionState;
};
