#pragma once

#include <exprtk.hpp>

#include <rules/Rule.h>
#include <things/ThingsRepository.h>

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

    Rule* onOffRule() const;

private:
    ThingsRepository _repo;
    exprtk::parser<double> _parser;
    exprtk::symbol_table<double> _symbolTable;
    std::unique_ptr<Rule> _onOffRule;
};
