#pragma once

#include <uvw_iot/ThingRepository.h>

using uvw_iot::ThingPtr;
using uvw_iot::ThingRepository;

class ConfigRepository;
class RuleEngine;
class Strategy;
class SymbolRepository;

class StrategyFactory {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const SymbolRepository& symbolRepository,
                                          const RuleEngine& rules,
                                          const ConfigRepository& cfg);
};
