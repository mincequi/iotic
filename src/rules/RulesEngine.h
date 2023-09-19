#pragma once

#include <list>
#include <memory>
#include <set>

#include <rules/RuleFactory.h>

class Rule;
class ThingsRepository;

class RulesEngine {
public:
    RulesEngine(const ThingsRepository& thingsRepository);
    ~RulesEngine();

private:
    // TODO: this does not work: raw pointer gets invalidated from time to time
    //void subscribe(const Thing* thing);
    // Using const ThingPtr& instead works.
    void subscribe(const ThingPtr& thing);
    void addDependency(const std::string& id);
    void subscribeDependencies();

    const ThingsRepository& _thingsRepository;
    RuleFactory _ruleFactory;
    std::list<std::unique_ptr<Rule>> _rules;
    std::set<std::string> _dependentThings;
    std::set<std::string> _subscribedThings;
    std::set<std::string> _subscribedVars;
};