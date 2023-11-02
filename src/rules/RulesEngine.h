#pragma once

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <rules/StrategyFactory.h>
#include <things/Thing.h>

class Strategy;
class ThingsRepository;
class te_parser;

#define rules RulesEngine::instance()

class RulesEngine {
public:
    static RulesEngine* instance();

    bool containsSymbol(const std::string& symbol);
    double resolveSymbol(const std::string& symbol);

    std::unique_ptr<te_parser> createParser(const std::string& expr);

private:
    RulesEngine(const ThingsRepository& thingsRepository);

    void subscribeDependencies();
    void subscribe(const ThingPtr& thing);
    void addDependency(const std::string& id);

    static inline RulesEngine* _instance = nullptr;

    const ThingsRepository& _thingsRepository;

    std::map<std::string, double> _symbolTable;

    std::list<std::unique_ptr<Strategy>> _strategies;
    std::set<std::string> _dependentThings;
    std::set<std::string> _subscribedThings;

    friend class TestUtil;
};
