#include "StrategyFactory.h"

#include <config/ConfigRepository.h>
#include <rules/RuleActuationStrategy.h>
#include <strategies/EvseStrategy.h>

std::unique_ptr<Strategy> StrategyFactory::from(const ThingPtr& thing,
                                                const ThingRepository& repo,
                                                const SymbolRepository& symbolRepository,
                                                const RuleEngine& rules,
                                                const ConfigRepository& cfg ) {
    auto strategy = EvseStrategy::from(thing, repo, cfg);
    if (!strategy) {
        strategy = RuleActuationStrategy::from(thing, repo, symbolRepository, rules, cfg);
    }


    if (strategy) {
        const auto priority = cfg.valueOr<int>(thing->id(), ConfigRepository::Key::priority);
        strategy->_priority = priority;
        return strategy;
    }

    return {};
}
