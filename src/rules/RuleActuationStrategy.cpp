#include "RuleActuationStrategy.h"

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>

#include <tinyexpr.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <config/Config.h>
#include <rules/RulesEngine.h>
#include <things/ThingsRepository.h>

#include "test_scheduler.hpp"

using namespace std::chrono_literals;

template<rpp::schedulers::constraint::scheduler TScheduler>
std::unique_ptr<Strategy> RuleActuationStrategy<TScheduler>::from(const ThingPtr& thing) {
    // Check if thing has "on" and "off" expression
    const auto onExpressionStr = cfg->valueOr<std::string>(thing->id(), Config::Key::on);
    if (onExpressionStr.empty()) return {};
    const auto offExpressionStr = cfg->valueOr<std::string>(thing->id(), Config::Key::off);
    if (offExpressionStr.empty()) return {};

    auto onExpression = rules->createParser(onExpressionStr);
    if (!onExpression) {
        LOG_S(ERROR) << thing->id() << "> error creating RuleActuationStrategy";
        return {};
    }
    auto offExpression = rules->createParser(offExpressionStr);
    if (!offExpression) {
        LOG_S(ERROR) << thing->id() << "> error creating RuleActuationStrategy";
        return {};
    }

    if (rules->containsSymbol(thing->id() + ".offset")) {
        thing->setProperty(MutableProperty::offset, defaultOffset);
    }

    LOG_S(INFO) << thing->id() <<
                   "> onExpression: " << onExpressionStr <<
                   ", offExpression: " << offExpressionStr;

    return std::make_unique<RuleActuationStrategy<rpp::schedulers::new_thread>>(thing->id(), std::move(onExpression), std::move(offExpression));
}

template<rpp::schedulers::constraint::scheduler TScheduler>
RuleActuationStrategy<TScheduler>::RuleActuationStrategy(const std::string& thingId,
                                                         std::unique_ptr<te_parser> onExpression,
                                                         std::unique_ptr<te_parser> offExpression) :
    Strategy(thingId),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)) {
    _expressionSubject.get_observable()
            .distinct_until_changed()
            .debounce(std::chrono::seconds(cfg->valueOr<int>(thingId, Config::Key::debounce, 60)), _scheduler)
            .observe_on(rppqt::schedulers::main_thread_scheduler{})
            .subscribe([this](bool value) {
        LOG_S(INFO) << this->thingId() << "> " << value;
        _actionState = value;
    });
}

template<rpp::schedulers::constraint::scheduler TScheduler>
void RuleActuationStrategy<TScheduler>::evaluate() {
    // 1. Check for off condition
    if (_offExpression->evaluate()) {
        _expressionSubject.get_subscriber().on_next(false);
        // 1.1 Check if this is the initial state
        if (!_actionState.has_value()) _actionState = false;
        // 2. Check for on condition
    } else if (_onExpression->evaluate()) {
        _expressionSubject.get_subscriber().on_next(true);
        // 2.1 Check if this is the initial state
        if (!_actionState.has_value()) _actionState = true;
        // 3. No condition matched. Retrigger last state (if present)
    } else if (_actionState.has_value()) {
        _expressionSubject.get_subscriber().on_next(_actionState.value());
    }

    if (_actionState.has_value()) {
        repo->setThingProperty(thingId(), MutableProperty::power_control, _actionState.value());
    }
}

// Explicit template instantiation
template class RuleActuationStrategy<rpp::schedulers::new_thread>;
template class RuleActuationStrategy<test_scheduler>;
