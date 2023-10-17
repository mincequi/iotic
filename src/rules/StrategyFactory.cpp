#include "StrategyFactory.h"

#include <limits>

#include <exprtk.hpp>
#include <common/Logger.h>
#include <config/Config.h>
#include <strategies/EvseStrategy.h>
#include <things/ThingsRepository.h>

#include "GenericActuationStrategy.h"

static exprtk::parser<double> parser;

StrategyFactory::StrategyFactory(exprtk::symbol_table<double>& symbolTable,
                         const ThingsRepository& thingsRepository) :
    _symbolTable(symbolTable),
    _thingsRepository(thingsRepository) {
    parser.enable_unknown_symbol_resolver();
}

std::unique_ptr<Strategy> StrategyFactory::from(const ThingPtr& thing) const {
    auto strategy = EvseStrategy::from(thing);
    if (strategy) return strategy;

    // TODO: move the on/off rule check to GenericActuationStrategy
    // Check if thing has "on" and "off" expression
    const auto onExpressionStr = cfg->valueOr<std::string>(thing->id(), Config::Key::on);
    if (onExpressionStr.empty()) return {};
    const auto offExpressionStr = cfg->valueOr<std::string>(thing->id(), Config::Key::off);
    if (offExpressionStr.empty()) return {};

    auto onExpression = std::make_unique<exprtk::expression<double>>();
    onExpression->register_symbol_table(_symbolTable);
    if (!parser.compile(onExpressionStr, *onExpression)) {
        LOG_S(ERROR) << thing->id() << "> " << parser.error();
        return {};
    }

    auto offExpression = std::make_unique<exprtk::expression<double>>();
    offExpression->register_symbol_table(_symbolTable);
    if (!parser.compile(offExpressionStr, *offExpression)) {
        LOG_S(ERROR) << thing->id() << "> " << parser.error();
        return {};
    }

    if (_symbolTable.is_variable(thing->id() + ".offset")) {
        thing->setProperty(MutableProperty::offset, 4.0);
    }

    return std::make_unique<GenericActuationStrategy<rpp::schedulers::new_thread>>(thing->id(), std::move(onExpression), std::move(offExpression));
}
