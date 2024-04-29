#include "RulesEngine.h"

#include <map>
#include <tinyexpr.h>
#include <uvw_iot/common/ThingRepository.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/Util.h>
#include <strategies/Strategy.h>
#include <things/Site.h>
#include <things/ThingValue.h>

using namespace std::placeholders;
using namespace uvw_iot::common;

double toDouble(const ThingPropertyValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    return 0.0;
}

RulesEngine::RulesEngine(const ThingRepository& thingRepository,
                         const Site& site,
                         const Config& cfg) :
    _thingRepository(thingRepository),
    _site(site),
    _cfg(cfg) {
    _site.gridPower().subscribe([this](int power) {
        _symbolTable["grid_power"] = power;
        // TODO: move strategy collection out of rules engine.
        // After update of site, evaluate strategies
        for (const auto& s : _strategies) {
            s->evaluate(power);
        }
    });

    // For each new thing, we potentially subscribe
    _thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
        // Each new thing might be a dependency for previous rules
        subscribeDependencies();

        // Check if thing has strategies
        auto strategy = StrategyFactory::from(thing, _thingRepository, _site, *this, _cfg);
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

    _thingRepository.thingRemoved().subscribe([this](const auto& id) {
        std::erase_if(_strategies, [&](const auto& t) {
            return t->thingId() == id;
        });
    });
}

bool RulesEngine::containsSymbol(const std::string& symbol) const {
    return _symbolTable.contains(symbol);
}

double RulesEngine::resolveSymbol(const std::string& symbol) const {
    LOG_S(INFO) << "resolveSymbol: " << symbol;
    return _symbolTable[symbol];
}

std::unique_ptr<te_parser> RulesEngine::createParser(const std::string& expr) const {
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
        if (_thingRepository.things().contains(*it)) {
            subscribe(_thingRepository.things().at(*it));
            it = _dependentThings.erase(it);
            continue;
        }
        ++it;
    }
}

void RulesEngine::subscribe(ThingPtr thing) {
    if (_subscribedThings.contains(thing->id())) return;

    LOG_S(INFO) << "subscribe to dependent thing: " << thing->id();
    thing->propertiesObservable().subscribe([this, thing](const ThingPropertyMap& prop) {
        for (const auto& kv : prop) {
            const std::string var = thing->id() + "." + util::toString(kv.first);
            if (_symbolTable.contains(var)) {
                switch (kv.first) {
                case ThingPropertyKey::offset:
                    _symbolTable[var] = offsetTable[std::get<int>(kv.second)];
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
