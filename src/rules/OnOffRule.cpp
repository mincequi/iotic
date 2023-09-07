#include "OnOffRule.h"

#include <exprtk.hpp>
#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>

#include <common/Logger.h>
#include <config/Config.h>

using namespace std::chrono_literals;

OnOffRule::OnOffRule(const std::string& id, ExprPtr&& onExpression, ExprPtr&& offExpression, Action action) :
    Rule(id),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)),
    _action(action) {
    _subject.get_observable()
            .distinct_until_changed()
            .debounce(std::chrono::milliseconds(cfg->valueOr<int>(id, Config::Key::debounce, 60000)), rpp::schedulers::new_thread{})
            .observe_on(rppqt::schedulers::main_thread_scheduler{})
            .subscribe([this](bool isOn) {
        LOG_S(INFO) << this->id() << "> " << isOn;
        _action(isOn);
    });
}

OnOffRule::~OnOffRule() {
}

void OnOffRule::evaluate() const {
    if (_offExpression->value()) {
        _subject.get_subscriber().on_next(false);
    } else if (_onExpression->value()) {
        _subject.get_subscriber().on_next(true);
    }
}
