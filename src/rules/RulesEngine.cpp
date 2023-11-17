#include "RulesEngine.h"

#include <map>
#include <tinyexpr.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/Util.h>
#include <strategies/Strategy.h>
#include <things/Site.h>
#include <things/ThingsRepository.h>

using namespace std::placeholders;

RulesEngine* RulesEngine::instance() {
    if (!_instance) _instance = new RulesEngine(*repo);
    return _instance;
}

RulesEngine::RulesEngine(const ThingsRepository& thingsRepository) :
    _thingsRepository(thingsRepository) {
    _site->properties().subscribe([this](const auto& prop) {
        for (const auto& kv : prop) {
            switch (kv.first) {
            case Property::pv_power:   _symbolTable["pv_power"] = std::get<double>(kv.second); break;
            case Property::grid_power: _symbolTable["grid_power"] = std::get<double>(kv.second); break;
            case Property::site_power: _symbolTable["site_power"] = std::get<double>(kv.second); break;
            default: break;
            }
        }
        // TODO: move strategy collection out of rules engine.
        // After update of site, evaluate strategies
        for (const auto& s : _strategies) {
            s->evaluate(prop);
        }
    });

    // For each new thing, we potentially subscribe
    _thingsRepository.thingAdded().subscribe([this](const ThingPtr& thing) {
        // Each new thing might be a dependency for previous rules
        subscribeDependencies();

        // Check if thing has strategies
        auto strategy = StrategyFactory::from(thing);
        if (!strategy) return;
        _strategies.push_back(std::move(strategy));

        // Creating rule might have updated the symbol table
        for (const auto& kv : _symbolTable) {
            addDependency(kv.first.substr(0, kv.first.find(".")));
        }
        addDependency(thing->id());

        // Subscribe new thing and dependencies
        subscribeDependencies();
    });

    _thingsRepository.thingRemoved().subscribe([this](const auto& id) {
        std::erase_if(_strategies, [&, this](const auto& t) {
            return t->thingId() == id;
        });
    });
}

bool RulesEngine::containsSymbol(const std::string& symbol) {
    return _symbolTable.contains(symbol);
}

double RulesEngine::resolveSymbol(const std::string& symbol) {
    LOG_S(INFO) << "resolveSymbol: " << symbol;
    return _symbolTable[symbol];
}

std::unique_ptr<te_parser> RulesEngine::createParser(const std::string& expr) {
    auto parser = std::make_unique<te_parser>();
    parser->set_unknown_symbol_resolver([this](std::string_view symbol) {
        return resolveSymbol(std::string(symbol));
    }, false); // <- we must set false here for further usage of parser
    if (!parser->compile(expr)) {
        LOG_S(ERROR) << "error parsing expression: " << parser->get_last_error_message();
        return {};
    }

    // We set the symbol table to the parser
    for (const auto& kv : _symbolTable) {
        auto var = std::string(kv.first);
        LOG_S(1) << "add variable: " << var;
        parser->add_variable_or_function({ var, &kv.second });
    }
    // We must recompile parser
    parser->compile(expr);

    return parser;
}

void RulesEngine::subscribeDependencies() {
    for (auto it = _dependentThings.begin(); it != _dependentThings.end();) {
        const auto& thing = _thingsRepository.thingById(std::string(*it));
        if (thing) {
            subscribe(thing);
            it = _dependentThings.erase(it);
            continue;
        };
        ++it;
    }
}

void RulesEngine::subscribe(const ThingPtr& thing) {
    if (_subscribedThings.contains(thing->id())) return;

    LOG_S(INFO) << "subscribe to dependent thing: " << thing->id();
    thing->properties().subscribe([this, &thing](const std::map<Property, Value>& prop) {
        for (const auto& kv : prop) {
            const std::string var = thing->id() + "." + util::toString(kv.first);
            if (_symbolTable.contains(var)) {
                switch (kv.first) {
                case Property::offset:
                    _symbolTable[var] = offsetTable[std::get<double>(kv.second)];
                    break;
                default:
                    _symbolTable[var] = toDouble(kv.second);
                    break;
                }
            }
        }
    });
    _subscribedThings.insert(thing->id());
}

void RulesEngine::addDependency(const std::string& id) {
    if (_subscribedThings.contains(id)) return;

    _dependentThings.insert(id);
}
