#include "RuleActuationStrategy.h"

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread.hpp>

#include <tinyexpr.h>

#include <uvw_iot/ThingRepository.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/RppUvw.h>
#include <config/ConfigRepository.h>
#include <rules/RulesEngine.h>
#include <rules/RuleUtil.h>
#include <rules/SymbolRepository.h>

using namespace std::chrono_literals;
using namespace uvw_iot;

template<rpp::schedulers::constraint::scheduler TScheduler>
std::unique_ptr<Strategy> RuleActuationStrategy<TScheduler>::from(const ThingPtr& thing,
                                                                  const ThingRepository& repo,
                                                                  const SymbolRepository& symbolRepository,
                                                                  const RuleEngine& ruleEngine,
                                                                  const ConfigRepository& cfg) {
    // Check if thing has "on" and "off" expression
    const auto onExpressionStr = cfg.valueOr<std::string>(thing->id(), ConfigRepository::Key::on);
    if (onExpressionStr.empty()) return {};
    const auto offExpressionStr = cfg.valueOr<std::string>(thing->id(), ConfigRepository::Key::off);
    if (offExpressionStr.empty()) return {};

    auto onExpression = ruleEngine.createParser(onExpressionStr);
    if (!onExpression) {
        LOG_S(ERROR) << thing->id() << "> error creating RuleActuationStrategy";
        return {};
    }
    auto offExpression = ruleEngine.createParser(offExpressionStr);
    if (!offExpression) {
        LOG_S(ERROR) << thing->id() << "> error creating RuleActuationStrategy";
        return {};
    }

    if (symbolRepository.containsSymbol(thing->id() + ".offset")) {
        thing->setProperty(ThingPropertyKey::offset, defaultOffset);
    }

    LOG_S(INFO) << thing->id() <<
                   "> onExpression: " << onExpressionStr <<
                   ", offExpression: " << offExpressionStr;

    return std::make_unique<RuleActuationStrategy<rpp::schedulers::new_thread>>(thing->id(),
                                                                                std::move(onExpression),
                                                                                std::move(offExpression),
                                                                                repo,
                                                                                cfg);
}

template<rpp::schedulers::constraint::scheduler TScheduler>
RuleActuationStrategy<TScheduler>::RuleActuationStrategy(const std::string& thingId,
                                                         std::unique_ptr<te_parser> onExpression,
                                                         std::unique_ptr<te_parser> offExpression,
                                                         const ThingRepository& repo,
                                                         const ConfigRepository& cfg) :
    Strategy(thingId),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)),
    _repo(repo),
    _cfg(cfg) {
    _expressionSubject.get_observable()
            | distinct_until_changed()
            | debounce(std::chrono::seconds(_cfg.valueOr<int>(thingId, ConfigRepository::Key::debounce, 60)), _scheduler)
            | observe_on(rpp_uvw::schedulers::main_thread_scheduler{})
            | subscribe([this](bool value) {
        LOG_S(INFO) << this->thingId() << "> " << value;
        _actionState = value;
    });
}

template<rpp::schedulers::constraint::scheduler TScheduler>
void RuleActuationStrategy<TScheduler>::evaluate(const Site::Properties& /*siteProperties*/) const {
    // 1. Check for off condition
    if (_offExpression->evaluate()) {
        _expressionSubject.get_observer().on_next(false);
        // 1.1 Check if this is the initial state
        if (!_actionState.has_value()) _actionState = false;
        // 2. Check for on condition
    } else if (_onExpression->evaluate()) {
        _expressionSubject.get_observer().on_next(true);
        // 2.1 Check if this is the initial state
        if (!_actionState.has_value()) _actionState = true;
        // 3. No condition matched. Retrigger last state (if present)
    } else if (_actionState.has_value()) {
        _expressionSubject.get_observer().on_next(_actionState.value());
    }

    if (_actionState.has_value()) {
        _repo.setThingProperty(thingId(), ThingPropertyKey::power_control, _actionState.value());
    }
}

template<rpp::schedulers::constraint::scheduler TScheduler>
json RuleActuationStrategy<TScheduler>::toJson() const {
    json j;
    j["type"] = "RuleActuationStrategy";
    j["on"] = rule::toJson(*_onExpression);
    j["off"] = rule::toJson(*_offExpression);
    return j;
}

// Explicit template instantiation
template class RuleActuationStrategy<rpp::schedulers::new_thread>;
