#include "RuleFactory.h"

#include <limits>

#include <exprtk.hpp>
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
    // Check if thing has "on" and "off" expression
    const auto onExpressionStr = cfg->valueOr<std::string>(thing->id(), Config::Key::on);
    if (onExpressionStr.empty()) return {};
    const auto offExpressionStr = cfg->valueOr<std::string>(thing->id(), Config::Key::off);
    if (offExpressionStr.empty()) return {};

    auto onExpression = std::make_unique<exprtk::expression<double>>();
    onExpression->register_symbol_table(_symbolTable);
    if (!parser.compile(onExpressionStr, *onExpression)) return {};

    auto offExpression = std::make_unique<exprtk::expression<double>>();
    offExpression->register_symbol_table(_symbolTable);
    if (!parser.compile(offExpressionStr, *offExpression)) return {};

    const auto offset = cfg->valueOr<int>(thing->id(), Config::Key::offset, std::numeric_limits<int>::max());
    if (offset != std::numeric_limits<int>::max()) {
        //thing->setProperty(WriteableThingProperty::power_offset, )
        //_thingsRepository.setThingProperty(thing->id(), WriteableThingProperty::power_control)
    }

    return std::make_unique<OnOffRule>(thing->id(), std::move(onExpression), std::move(offExpression), [&](bool isOn) {
        _thingsRepository.setThingProperty(thing->id(), MutableProperty::power_control, isOn);
    });
}
