#pragma once

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <uvw_iot/Thing.h>

#include <strategies/Strategy.h>
#include <rules/StrategyFactory.h>

class te_parser;
namespace uvw_iot {
    class ThingRepository;
}

using uvw_iot::ThingRepository;
class SymbolRepository;
class StrategyRepository;

class RuleEngine {
public:
    RuleEngine(const ThingRepository& thingRepository,
                StrategyRepository& strategyRepository,
                const SymbolRepository& symbolRepository,
                const ConfigRepository& cfg);
    ~RuleEngine() = default;

    std::unique_ptr<te_parser> createParser(const std::string& expr) const;

private:
    void subscribeDependencies();
    void subscribe(ThingPtr thing);
    void addDependency(const std::string& id);

    const ThingRepository& _thingRepository;
    StrategyRepository& _strategyRepository;
    const SymbolRepository& _symbolRepository;
    const ConfigRepository& _configRepository;

    std::set<std::string> _dependentThings;
    std::set<std::string> _subscribedThings;

    friend class TestUtil;
};
