#pragma once

#include <memory>
#include <optional>

#include <uvw_iot/ThingRepository.h>

#include <common/Rpp.h>
#include <strategies/Strategy.h>

using uvw_iot::ThingPtr;
using uvw_iot::ThingRepository;

class te_parser;
class ConfigRepository;
class RuleEngine;
class SymbolRepository;

template<rpp::schedulers::constraint::scheduler TScheduler>
class RuleActuationStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const SymbolRepository& symbolRepository,
                                          const RuleEngine& rules,
                                          const ConfigRepository& cfg);

    RuleActuationStrategy(const std::string& thingId,
                          std::unique_ptr<te_parser> onExpression,
                          std::unique_ptr<te_parser> offExpression,
                          const ThingRepository& repo,
                          const ConfigRepository& cfg);

    void evaluate(const Site::Properties& siteProperties) const override;
    bool wantsToTurnOff(const Site::Properties& siteProperties) override;
    bool wantsToTurnOn(const Site::Properties& siteProperties) override;

    json toJson() const override;

private:
    bool actuate();

    const ThingRepository& _thingRepository;
    const ConfigRepository& _cfg;

    std::unique_ptr<te_parser> _onExpression;
    std::unique_ptr<te_parser> _offExpression;
    TScheduler _scheduler;
    publish_subject<bool> _actuationSubject;
    mutable std::optional<bool> _actuationState;
    mutable std::optional<bool> _nextActuationState;
    std::chrono::time_point<std::chrono::system_clock> _lastActuationTs;
    mutable bool _isAlreadySet = false;
};
