#include "StrategyRepository.h"

#include <strategies/Strategy.h>
#include <uvw_iot/ThingRepository.h>

StrategyRepository::StrategyRepository(const uvw_iot::ThingRepository& thingRepository)
    : _thingRepository(thingRepository),
    _strategiesSubject(std::list<std::shared_ptr<Strategy>>()) {

    _thingRepository.thingRemoved().subscribe([this](const auto& id) {
        std::erase_if(_strategies, [&](const auto& t) {
            return t->thingId() == id;
        });
    });
}

void StrategyRepository::addStrategy(std::shared_ptr<Strategy> strategy) {
    _strategies.push_back(std::move(strategy));

    // Sort strategies by priority (highest priority first)
    _strategies.sort([](const auto& a, const auto& b) {
        return a->priority() > b->priority();
    });
}
