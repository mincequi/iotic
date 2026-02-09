#include "RuleActuationStrategy.h"

#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/schedulers/new_thread.hpp>

#include <tinyexpr.h>

#include <uvw_iot/ThingRepository.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/RppUvw.h>
#include <config/ConfigRepository.h>
#include <rules/RuleEngine.h>
#include <rules/RuleUtil.h>
#include <rules/SymbolRepository.h>

using namespace std::chrono_literals;
using namespace uvw_iot;

std::unique_ptr<Strategy> RuleActuationStrategy::from(
    const ThingPtr& thing,
    const ThingRepository& thingRepository,
    const SymbolRepository& symbolRepository,
    const RuleEngine& ruleEngine,
    const ConfigRepository& configRepository) {

    // Check if thing has "on" and "off" expression
    const auto onExpressionStr = configRepository.valueOr<std::string>(thing->id(), ConfigRepository::Key::on);
    if (onExpressionStr.empty()) return {};
    const auto offExpressionStr = configRepository.valueOr<std::string>(thing->id(), ConfigRepository::Key::off);
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

    // TODO: why handle offset here? It shall be defined in the expressions.
    if (symbolRepository.containsSymbol(thing->id() + ".offset")) {
        ThingPropertyMap properties;
        properties.set<ThingPropertyKey::offset>(defaultOffset);
        thing->setProperties(properties);
    }

    LOG_S(INFO) << thing->id() <<
                   "> onExpression: " << onExpressionStr <<
                   ", offExpression: " << offExpressionStr;

    return std::unique_ptr<RuleActuationStrategy>(
        new RuleActuationStrategy(
            thing,
            std::move(onExpression),
            std::move(offExpression),
            thingRepository)
        );
}

RuleActuationStrategy::RuleActuationStrategy(const ThingPtr& thing,
                                             std::unique_ptr<te_parser> onExpression,
                                             std::unique_ptr<te_parser> offExpression,
                                             const ThingRepository& thingRepository) :
    Strategy(thing->id()),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)),
    _thingRepository(thingRepository) {

    // Subscribe to thing's power
    thing->propertiesObservable().subscribe([this](const ThingPropertyMap& map) {
        map.on<ThingPropertyKey::power>([&](const auto& value) {
            _measuredPower = value;
        });
    });
}

json RuleActuationStrategy::toJson() const {
    json j;
    j["thingId"] = thingId();
    j["type"] = "RuleActuationStrategy";
    j["priority"] = priority();
    j["state"] = _state;
    j["nextState"] = _nextState;
    //j["on"] = rule::toJson(*_onExpression);
    //j["off"] = rule::toJson(*_offExpression);
    return j;
}

bool RuleActuationStrategy::wantsToStepDown(const Site::Properties&) const {
    if (_offExpression->evaluate()) {
        _nextState = false;
    }

    return _nextState.has_value() && !_nextState.value() && (_state != _nextState) ;
}

bool RuleActuationStrategy::wantsToStepUp(const Site::Properties&) const {
    if (_onExpression->evaluate()) {
        _nextState = true;
    }

    return _nextState.has_value() && _nextState.value() && (_state != _nextState) ;
}

void RuleActuationStrategy::adjust(Step step, const Site::Properties&) {
    switch (step) {
    case Step::Keep:
        break;
    case Step::Down:
        _state = false;
        LOG_S(INFO) << this->thingId() << "> " << _state.value();
        break;
    case Step::Up:
        _state = true;
        LOG_S(INFO) << this->thingId() << "> " << _state.value();
        break;
    }

    if (_state.has_value()) {
        ThingPropertyMap properties;
        properties.set<ThingPropertyKey::power_control>(_state.value());
        _thingRepository.setThingProperties(thingId(), properties);
    }
}
