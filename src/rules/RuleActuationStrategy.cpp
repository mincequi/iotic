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

    return std::make_unique<RuleActuationStrategy<rpp::schedulers::new_thread>>(thing->id(),
                                                                                std::move(onExpression),
                                                                                std::move(offExpression),
                                                                                thingRepository,
                                                                                cfg);
}

template<rpp::schedulers::constraint::scheduler TScheduler>
RuleActuationStrategy<TScheduler>::RuleActuationStrategy(const std::string& thingId,
                                                         std::unique_ptr<te_parser> onExpression,
                                                         std::unique_ptr<te_parser> offExpression,
                                                         const ThingRepository& thingRepository,
                                                         const ConfigRepository& cfg) :
    Strategy(thingId),
    _onExpression(std::move(onExpression)),
    _offExpression(std::move(offExpression)),
    _thingRepository(thingRepository),
    _cfg(cfg) {
    _actuationSubject.get_observable()
            | distinct_until_changed()
            | debounce(std::chrono::seconds(_cfg.valueOr<int>(thingId, ConfigRepository::Key::debounce, 60)), _scheduler)
            | observe_on(rpp_uvw::schedulers::main_thread_scheduler{})
            | subscribe([this](bool value) {
        LOG_S(INFO) << this->thingId() << "> " << value;
        _actuationState = value;
    });
}

template<rpp::schedulers::constraint::scheduler TScheduler>
void RuleActuationStrategy<TScheduler>::evaluate(const Site::Properties& siteProperties) const {
    // TODO: temporary disable actuation
    return;

    // 1. Check for off condition
    if (_offExpression->evaluate()) {
        _actuationSubject.get_observer().on_next(false);
        // 1.1 Check if this is the initial state
        if (!_actuationState.has_value()) _actuationState = false;
        // 2. Check for on condition
    } else if (_onExpression->evaluate()) {
        _actuationSubject.get_observer().on_next(true);
        // 2.1 Check if this is the initial state
        if (!_actuationState.has_value()) _actuationState = true;
        // 3. No condition matched. Retrigger last state (if present)
    } else if (_actuationState.has_value()) {
        _actuationSubject.get_observer().on_next(_actuationState.value());
    }

    // We actually have to feed the state to the thing, which is responsible to actually set it on the device
    if (_actuationState.has_value()) {
        _thingRepository.setThingProperty(thingId(), ThingPropertyKey::power_control, _actuationState.value());
    }
}

template<rpp::schedulers::constraint::scheduler TScheduler>
bool RuleActuationStrategy<TScheduler>::wantsToTurnOff(const Site::Properties& siteProperties) {
    if (_offExpression->evaluate()) {
        _nextActuationState = false;
    }
    _isAlreadySet = false;
    return actuate();
}

template<rpp::schedulers::constraint::scheduler TScheduler>
bool RuleActuationStrategy<TScheduler>::wantsToTurnOn(const Site::Properties& siteProperties) {
    if (_onExpression->evaluate()) {
        _nextActuationState = true;
    }
    return actuate();
}

template<rpp::schedulers::constraint::scheduler TScheduler>
json RuleActuationStrategy<TScheduler>::toJson() const {
    json j;
    j["type"] = "RuleActuationStrategy";
    j["on"] = rule::toJson(*_onExpression);
    j["off"] = rule::toJson(*_offExpression);
    return j;
}

template<rpp::schedulers::constraint::scheduler TScheduler>
bool RuleActuationStrategy<TScheduler>::actuate() {
    if (!_nextActuationState.has_value()) return false;

    const bool wantsToActuate = _actuationState != _nextActuationState;
    const auto now = std::chrono::system_clock::now();
    if ((_nextActuationState != _actuationState) && (_lastActuationTs + std::chrono::seconds(180) > now)) {
        _lastActuationTs = now;
        _actuationState = _nextActuationState;
    }

    if (_actuationState == _nextActuationState && !_isAlreadySet) {
        _thingRepository.setThingProperty(thingId(), ThingPropertyKey::power_control, _actuationState.value());
        _isAlreadySet = true;
    }

    return wantsToActuate;
}

// Explicit template instantiation
template class RuleActuationStrategy<rpp::schedulers::new_thread>;
