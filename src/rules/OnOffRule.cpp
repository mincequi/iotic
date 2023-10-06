#include "OnOffRule.h"

#include <exprtk.hpp>
#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/map.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>

#include <common/Logger.h>
#include <config/Config.h>

#include "test_scheduler.hpp"

using namespace std::chrono_literals;

template<rpp::schedulers::constraint::scheduler TScheduler>
OnOffRule<TScheduler>::OnOffRule(const std::string& thingId,
                     ExprPtr&& onExpression,
                     ExprPtr&& offExpression,
                     Action action) :
    Rule(thingId),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)),
    _action(action) {
    _expressionSubject.get_observable()
            .distinct_until_changed()
            .debounce(std::chrono::milliseconds(cfg->valueOr<int>(thingId, Config::Key::debounce, 60000)), _scheduler)
            .observe_on(rppqt::schedulers::main_thread_scheduler{})
            .subscribe([this](bool value) {
        LOG_S(INFO) << this->thingId() << "> " << value;
        _actionState = value;
    });
}

template<rpp::schedulers::constraint::scheduler TScheduler>
OnOffRule<TScheduler>::~OnOffRule() {
}

template<rpp::schedulers::constraint::scheduler TScheduler>
void OnOffRule<TScheduler>::evaluate() {
    // 1. Check for off condition
    if (_offExpression->value()) {
        _expressionSubject.get_subscriber().on_next(false);
        // 1.1 Check if this is the initial state
        if (!_actionState.has_value()) _actionState = false;
    // 2. Check for on condition
    } else if (_onExpression->value()) {
        _expressionSubject.get_subscriber().on_next(true);
        // 2.1 Check if this is the initial state
        if (!_actionState.has_value()) _actionState = true;
    // 3. No condition matched. Retrigger last state (if present)
    } else if (_actionState.has_value()) {
        _expressionSubject.get_subscriber().on_next(_actionState.value());
    }

    if (_actionState.has_value()) {
        _action(_actionState.value());
    }
}

template<rpp::schedulers::constraint::scheduler TScheduler>
TScheduler& OnOffRule<TScheduler>::scheduler() {
    return _scheduler;
}

// Explicit template instantiation
template class OnOffRule<rpp::schedulers::new_thread>;
template class OnOffRule<test_scheduler>;
