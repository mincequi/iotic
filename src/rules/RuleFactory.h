#pragma once

#include <memory>

#include <things/Thing.h>

class Rule;
class ThingsRepository;
namespace exprtk {
template<class>
class symbol_table;
}

class RuleFactory {
public:
    RuleFactory(exprtk::symbol_table<double>& symbolTable,
                const ThingsRepository& thingsRepository);

    std::unique_ptr<Rule> from(const ThingPtr& thing) const;

private:
    exprtk::symbol_table<double>& _symbolTable;
    const ThingsRepository& _thingsRepository;
};
