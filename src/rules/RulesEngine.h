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
class StrategyRepository;

class RulesEngine {
public:
    RulesEngine(const ThingRepository& thingRepository,
                StrategyRepository& strategyRepository,
                const Site& site,
                const Config& cfg);
    ~RulesEngine() = default;

    bool containsSymbol(const std::string& symbol) const;
    double resolveSymbol(const std::string& symbol) const;
    inline const std::map<std::string, double>& symbolTable() const {
        return _symbolTable;
    }

    std::unique_ptr<te_parser> createParser(const std::string& expr) const;

private:
    void subscribeDependencies();
    void subscribe(ThingPtr thing);
    void addDependency(const std::string& id);

    const ThingRepository& _thingRepository;
    StrategyRepository& _strategyRepository;
    const Site& _site;
    const Config& _cfg;

    mutable std::map<std::string, double> _symbolTable;

    std::set<std::string> _dependentThings;
    std::set<std::string> _subscribedThings;

    friend class TestUtil;
};
