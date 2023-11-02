#include "StrategyFactory.h"

#include <rules/RuleActuationStrategy.h>
#include <strategies/EvseStrategy.h>

std::unique_ptr<Strategy> StrategyFactory::from(const ThingPtr& thing) {
    auto strategy = EvseStrategy::from(thing);
    if (strategy) return strategy;

    strategy = RuleActuationStrategy<rpp::schedulers::new_thread>::from(thing);
    if (strategy) return strategy;

    return {};
}
