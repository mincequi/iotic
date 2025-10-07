#include "RulesEngine.h"

#include <map>
#include <tinyexpr.h>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/util/Site.h>

#include <common/Logger.h>
#include <common/OffsetTable.h>
#include <common/Util.h>
#include <rules/SymbolRepository.h>
#include <strategies/Strategy.h>
#include <strategies/StrategyRepository.h>
#include <things/ThingValue.h>

using namespace std::placeholders;
using namespace uvw_iot;
using namespace uvw_iot::util;

double toDouble(const ThingPropertyValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    return 0.0;
}

RuleEngine::RuleEngine(const ThingRepository& thingRepository,
                         StrategyRepository& strategyRepository,
                         const SymbolRepository& symbolRepository,
                         const Site& site,
                         const ConfigRepository& cfg) :
    _thingRepository(thingRepository),
    _strategyRepository(strategyRepository),
    _symbolRepository(symbolRepository),
    _site(site),
    _cfg(cfg) {
    _site.properties().subscribe([this](const Site::Properties& props) {
        _symbolRepository.setSymbol("short_term_grid_power", props.shortTermGridPower);
        _symbolRepository.setSymbol("long_term_grid_power", props.longTermGridPower);
        _symbolRepository.setSymbol("grid_power", props.gridPower);
        _symbolRepository.setSymbol("pv_power", props.pvPower);

        // After update of site, evaluate strategies
        for (const auto& s : _strategyRepository.strategies()) {
            s->evaluate(props);
        }
    });

    // For each new thing, we potentially subscribe
    _thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
        // Each new thing might be a dependency for previous rules
        subscribeDependencies();

        // Check if thing has strategies
        auto strategy = StrategyFactory::from(thing, _thingRepository, _symbolRepository, *this, _cfg);
        if (!strategy) return;
        _strategyRepository.addStrategy(std::move(strategy));

        // Creating rule might have updated the symbol table
        for (const auto& kv : _symbolRepository.symbolTable()) {
            addDependency(kv.first.substr(0, kv.first.find(".")));
        }
        addDependency(thing->id());

        // Subscribe new thing and dependencies
        subscribeDependencies();
    });

    // For each removed thing, we unsubscribe
    _thingRepository.thingRemoved().subscribe([this](const std::string& id) {
        LOG_S(INFO) << "thing removed: " << id;
        _subscribedThings.erase(id);
    });
}

std::unique_ptr<te_parser> RuleEngine::createParser(const std::string& expr) const {
    auto parser = std::make_unique<te_parser>();
    parser->set_unknown_symbol_resolver([this](std::string_view symbol) {
        return _symbolRepository.resolveSymbol(std::string(symbol));
    }, false); // <- we must set false here for further usage of parser
    if (!parser->compile(expr)) {
        LOG_S(ERROR) << "error parsing expression: " << parser->get_last_error_message();
        return {};
    }

    // We set the symbol table to the parser
    for (const auto& kv : _symbolRepository.symbolTable()) {
        auto var = std::string(kv.first);
        LOG_S(1) << "add variable: " << var;
        parser->add_variable_or_function({ var, &kv.second });
    }
    // We must recompile parser
    parser->compile(expr);

    return parser;
}

void RuleEngine::subscribeDependencies() {
    for (auto it = _dependentThings.begin(); it != _dependentThings.end();) {
        if (_thingRepository.things().contains(*it)) {
            subscribe(_thingRepository.things().at(*it));
            it = _dependentThings.erase(it);
            continue;
        }
        ++it;
    }
}

void RuleEngine::subscribe(ThingPtr thing) {
    if (_subscribedThings.contains(thing->id())) return;

    LOG_S(INFO) << "subscribe to dependent thing: " << thing->id();
    thing->propertiesObservable().subscribe([this, thing](const ThingPropertyMap& prop) {
        for (const auto& kv : prop) {
            const std::string var = thing->id() + "." + ::util::toString(kv.first);
            //if (_symbolRepository.symbolTable().contains(var)) {
                switch (kv.first) {
                case ThingPropertyKey::offset:
                    _symbolRepository.setSymbol(var, offsetTable[std::get<int>(kv.second)]);
                    break;
                default:
                    _symbolRepository.setSymbol(var, toDouble(kv.second));
                    break;
                }
            //}
        }
    });
    _subscribedThings.insert(thing->id());
}

void RuleEngine::addDependency(const std::string& id) {
    if (_subscribedThings.contains(id)) return;

    _dependentThings.insert(id);
}
