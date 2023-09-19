#include "RulesEngine.h"

#include <common/Logger.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

#include <exprtk.hpp>

#include "Rule.h"

// TODO: for now our rules engine is based on exprtk. This might change.
// Check https://caiorss.github.io/C-Cpp-Notes/embedded_scripting_languages.html
static exprtk::symbol_table<double> primarySymbolTable;

RulesEngine::RulesEngine(const ThingsRepository& thingsRepository) :
    _thingsRepository(thingsRepository),
    _ruleFactory(primarySymbolTable, thingsRepository) {

    primarySymbolTable.create_variable(util::toString(Property::pv_power));
    primarySymbolTable.create_variable(util::toString(Property::grid_power));
    primarySymbolTable.create_variable(util::toString(Property::site_power));

    _thingsRepository.site().properties().subscribe([](const auto& prop) {
        for (const auto& kv : prop) {
            switch (kv.first) {
            case Property::pv_power:
            case Property::grid_power:
            case Property::site_power: {
                const std::string var = util::toString(kv.first);
                primarySymbolTable.variable_ref(var) = toDouble(kv.second);
                break;
            }
            default:
                break;
            }
        }
    });

    _thingsRepository.thingAdded().subscribe([this](const ThingPtr& thing) {
        // Each new thing might be a dependency for previous rules
        subscribeDependencies();

        // Check if thing has rules
        auto rule = _ruleFactory.from(thing);
        if (!rule) return;

        // Add new rule to list
        _rules.push_back(std::move(rule));

        // Creating rule might have updated the symbol table
        std::list<std::string> vars_;
        primarySymbolTable.get_variable_list(vars_);
        _subscribedVars = { vars_.begin(), vars_.end() };
        for (const auto& v : vars_) {
            addDependency(v.substr(0, v.find(".")));
        }
        addDependency(thing->id());

        // Subscribe new thing and dependencies
        subscribeDependencies();
    });

    // Use site properties as pulse generator
    _thingsRepository.site().properties().subscribe([this](const auto&) {
        for (const auto& r : _rules) {
            r->evaluate();
        }
    });
}

RulesEngine::~RulesEngine() {
}

void RulesEngine::subscribe(const ThingPtr& thing) {
    if (_subscribedThings.contains(thing->id())) return;

    thing->properties().subscribe([this, &thing](const std::map<Property, Value>& prop) {
        for (const auto& kv : prop) {
            const std::string var = thing->id() + "." + util::toString(kv.first);
            if (_subscribedVars.contains(var)) {
                primarySymbolTable.variable_ref(var) = toDouble(kv.second);
            }
        }
    });
    _subscribedThings.insert(thing->id());
}

void RulesEngine::addDependency(const std::string& id) {
    if (_subscribedThings.contains(id)) return;

    _dependentThings.insert(id);
}

void RulesEngine::subscribeDependencies() {
    for (auto it = _dependentThings.begin(); it != _dependentThings.end();) {
        const auto& thing = _thingsRepository.thingById(*it);
        if (thing) {
            subscribe(thing);
            it = _dependentThings.erase(it);
            continue;
        };
        ++it;
    }
}
