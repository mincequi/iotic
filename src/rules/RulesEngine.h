#pragma once

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <uvw_iot/common/Thing.h>

#include <strategies/Strategy.h>
#include <rules/StrategyFactory.h>

class Site;
class te_parser;
namespace uvw_iot::common {
    class ThingRepository;
}

using uvw_iot::common::ThingRepository;

class RulesEngine {
public:
    RulesEngine(const ThingRepository& thingRepository,
                const Site& site,
                const Config& cfg);
    ~RulesEngine() = default;

    bool containsSymbol(const std::string& symbol) const;
    double resolveSymbol(const std::string& symbol) const;

    std::unique_ptr<te_parser> createParser(const std::string& expr) const;

private:
    void subscribeDependencies();
    void subscribe(ThingPtr thing);
    void addDependency(const std::string& id);

    const ThingRepository& _thingRepository;
    const Site& _site;
    const Config& _cfg;

    mutable std::map<std::string, double> _symbolTable;

    std::list<std::unique_ptr<Strategy>> _strategies;
    std::set<std::string> _dependentThings;
    std::set<std::string> _subscribedThings;

    friend class TestUtil;
};
