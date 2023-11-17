#pragma once

#include <memory>
#include <optional>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <strategies/Strategy.h>
#include <things/Thing.h>

class te_parser;

template<rpp::schedulers::constraint::scheduler TScheduler>
class RuleActuationStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing);

    RuleActuationStrategy(const std::string& thingId,
                          std::unique_ptr<te_parser> onExpression,
                          std::unique_ptr<te_parser> offExpression);

    void evaluate(const std::map<Property, Value>& siteProperties) override;

private:
    std::unique_ptr<te_parser> _onExpression;
    std::unique_ptr<te_parser> _offExpression;
    TScheduler _scheduler;
    rpp::subjects::publish_subject<bool> _expressionSubject;
    std::optional<bool> _actionState;
};
