#include "PowerManager.h"

#include <uvw_iot/util/Site.h>

#include <config/ConfigRepository.h>
#include <rules/SymbolRepository.h>
#include <strategies/Strategy.h>
#include <strategies/StrategyRepository.h>

using namespace uvw_iot::util;

PowerManager::PowerManager(const StrategyRepository& strategyRepository,
                           const SymbolRepository& symbolRepository,
                           const Site& site,
                           const ConfigRepository& configRepository) {
    site.properties().subscribe([&](const Site::Properties& siteProperties) {
        // Update power-related symbols
        symbolRepository.setSymbol("short_term_grid_power", siteProperties.shortTermGridPower);
        symbolRepository.setSymbol("long_term_grid_power", siteProperties.longTermGridPower);
        symbolRepository.setSymbol("grid_power", siteProperties.gridPower);
        symbolRepository.setSymbol("pv_power", siteProperties.pvPower);

        // After update of site, evaluate strategies
        auto strategies = strategyRepository.strategies();

        // First, check for step down requests
        for (auto it = strategies.begin(); it != strategies.end(); ++it) {
            const auto& strategy = *it;
            if (strategy->wantsToStepDown(siteProperties) && siteProperties.ts >= _lastStepTs + ConfigRepository::stepDownDebounceSeconds) {
                _lastStepTs = siteProperties.ts;
                strategy->adjust(Strategy::Step::Down, siteProperties);
                // Remove strategy from list to avoid multiple steps in one cycle
                strategies.erase(it);
                break;
            }
        }

        // Then, check for step up requests in reverse order
        for (auto it = strategies.rbegin(); it != strategies.rend(); ++it) {
            const auto& strategy = *it;
            if (strategy->wantsToStepUp(siteProperties) && siteProperties.ts >= _lastStepTs + ConfigRepository::stepUpDebounceSeconds) {
                _lastStepTs = siteProperties.ts;
                strategy->adjust(Strategy::Step::Up, siteProperties);
                // Remove strategy from list to avoid multiple steps in one cycle
                strategies.erase(std::next(it).base());
                break;
            }
        }

        // No step, just modulate.
        // TODO: this currently only works with a single modulation strategy
        for (const auto& strategy : strategies) {
            strategy->adjust(Strategy::Step::Keep, siteProperties);
        }
    });
}
