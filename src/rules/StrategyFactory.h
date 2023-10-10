#pragma once

#include <memory>

#include <things/Thing.h>

class Strategy;
class ThingsRepository;
namespace exprtk {
template<class>
class symbol_table;
}

class StrategyFactory {
public:
    StrategyFactory(exprtk::symbol_table<double>& symbolTable,
                const ThingsRepository& thingsRepository);

    std::unique_ptr<Strategy> from(const ThingPtr& thing) const;

private:
    exprtk::symbol_table<double>& _symbolTable;
    const ThingsRepository& _thingsRepository;
};
