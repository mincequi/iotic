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

std::unique_ptr<Strategy> RuleActuationStrategy::from(const ThingPtr& thing,
                                                                  const ThingRepository& thingRepository,
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

    return std::make_unique<RuleActuationStrategy>(thing->id(),
                                                   std::move(onExpression),
                                                   std::move(offExpression),
                                                   thingRepository,
                                                   cfg);
}

RuleActuationStrategy::RuleActuationStrategy(const std::string& thingId,
                                             std::unique_ptr<te_parser> onExpression,
                                             std::unique_ptr<te_parser> offExpression,
                                             const ThingRepository& thingRepository,
                                             const ConfigRepository& cfg) :
    Strategy(thingId),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)),
    _thingRepository(thingRepository),
    _cfg(cfg) {
}

void RuleActuationStrategy::evaluate(const Site::Properties& siteProperties) const {
    // disabled for RuleActuationStrategy
    return;
}

bool RuleActuationStrategy::wantsToTurnOff(const Site::Properties& siteProperties) {
    if (_offExpression->evaluate()) {
        _nextActuationState = false;
    }
    _isAlreadySet = false;
    return actuate();
}

bool RuleActuationStrategy::wantsToTurnOn(const Site::Properties& siteProperties) {
    if (_onExpression->evaluate()) {
        _nextActuationState = true;
    }
    return actuate();
}

json RuleActuationStrategy::toJson() const {
    json j;
    j["thingId"] = thingId();
    j["type"] = "RuleActuationStrategy";
    j["priority"] = priority();
    //j["on"] = rule::toJson(*_onExpression);
    //j["off"] = rule::toJson(*_offExpression);
    return j;
}

bool RuleActuationStrategy::actuate() {
    if (!_nextActuationState.has_value()) return false;

    const bool wantsToActuate = _actuationState != _nextActuationState;
    const auto now = std::chrono::system_clock::now();
    if (wantsToActuate && (_lastActuationTs + std::chrono::seconds(180) < now)) {
        _lastActuationTs = now;
        _actuationState = _nextActuationState;
        LOG_S(INFO) << this->thingId() << "> " << _actuationState.value();
    }

    if (_actuationState == _nextActuationState && !_isAlreadySet) {
        _thingRepository.setThingProperty(thingId(), ThingPropertyKey::power_control, _actuationState.value());
        _isAlreadySet = true;
    }

    return wantsToActuate;
}
