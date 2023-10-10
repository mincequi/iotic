#pragma once

#include <exprtk.hpp>

#include <strategies/Strategy.h>
#include <things/Thing.h>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

class TestUtil {
public:
    TestUtil();

    void setProperty(const std::string& id,
                     Property property,
                     const Value& value);

    void setOnOffRule(const std::string& id,
                      const std::string& onExpressionStr,
                      const std::string& offExpressionStr);

    const ThingPtr& thingById(const std::string& id) const;

    Strategy* onOffRule() const;

private:
    exprtk::parser<double> _parser;
    exprtk::symbol_table<double> _symbolTable;
    std::unique_ptr<Strategy> _onOffRule;
};
