#include "RuleFactory.h"

#include <limits>

#include <exprtk.hpp>
#include <common/Logger.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

#include "OnOffRule.h"

static exprtk::parser<double> parser;

RuleFactory::RuleFactory(exprtk::symbol_table<double>& symbolTable,
                         const ThingsRepository& thingsRepository) :
    _symbolTable(symbolTable),
    _thingsRepository(thingsRepository) {
    parser.enable_unknown_symbol_resolver();
}

std::unique_ptr<Rule> RuleFactory::from(const ThingPtr& thing) const {
    // TODO: move the on/off rule check to OnOffRule
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

    // If we have a valid on and off rule, we provide offset property to thing
    thing->setProperty(MutableProperty::offset, 0.0);

    return std::make_unique<OnOffRule>(thing->id(), std::move(onExpression), std::move(offExpression), [&](bool isOn) {
        _thingsRepository.setThingProperty(thing->id(), MutableProperty::power_control, isOn);
    });
}
