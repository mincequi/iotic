#include "StrategyFactory.h"

#include <rules/RuleActuationStrategy.h>
#include <strategies/EvseStrategy.h>

std::unique_ptr<Strategy> StrategyFactory::from(const ThingPtr& thing,
                                                const ThingRepository& repo,
                                                const SymbolRepository& symbolRepository,
                                                const Site& site,
                                                const RuleEngine& rules,
                                                const ConfigRepository& cfg ) {
    auto strategy = EvseStrategy::from(thing, repo, site, cfg);
    if (strategy) return strategy;

    strategy = RuleActuationStrategy<rpp::schedulers::new_thread>::from(thing, repo, symbolRepository, rules, cfg);
    if (strategy) return strategy;

    return {};
}
