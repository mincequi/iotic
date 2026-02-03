#include "StrategyFactory.h"

#include <config/ConfigRepository.h>
#include <rules/RuleActuationStrategy.h>
#include <strategies/MultiPhaseStrategy.h>
#include <strategies/WallboxStrategy.h>

std::unique_ptr<Strategy> StrategyFactory::from(const ThingPtr& thing,
                                                const ThingRepository& thingRepository,
                                                const SymbolRepository& symbolRepository,
                                                const RuleEngine& ruleEngine,
                                                const ConfigRepository& configRepository) {
    auto strategy = WallboxStrategy::from(thing, thingRepository, configRepository);
    if (!strategy) {
        strategy = RuleActuationStrategy::from(thing, thingRepository, symbolRepository, ruleEngine, configRepository);
    }
    if (!strategy && thing->type() == uvw_iot::ThingType::Relay) {
        strategy = MultiPhaseStrategy::from(thing, configRepository);
    }

    if (strategy) {
        const auto priority = configRepository.valueOr<int>(thing->id(), ConfigRepository::Key::priority);
        strategy->_priority = priority;
        return strategy;
    }

    return {};
}
