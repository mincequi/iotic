#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <rpp/subjects/behavior_subject.hpp>
#include <rpp/subjects/publish_subject.hpp>
#include <rules/Rule.h>

namespace exprtk {
template<class>
class expression;
}

using ExprPtr = std::unique_ptr<exprtk::expression<double>>;
using Action = std::function<void(bool)>;

template<rpp::schedulers::constraint::scheduler TScheduler>
class OnOffRule : public Rule {
public:
    OnOffRule(const std::string& thingId,
              ExprPtr&& onExpression,
              ExprPtr&& offExpression,
              Action action);
    ~OnOffRule();

    void evaluate() override;

    TScheduler& scheduler();

private:
    ExprPtr _onExpression;
    ExprPtr _offExpression;
    Action _action;
    TScheduler _scheduler;
    rpp::subjects::publish_subject<bool> _expressionSubject;
    std::optional<bool> _actionState;
};
