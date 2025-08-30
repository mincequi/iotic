#pragma once

#include <uvw_iot/ThingRepository.h>

using uvw_iot::ThingPtr;
using uvw_iot::ThingRepository;

class ConfigRepository;
class RuleEngine;
class Strategy;
class SymbolRepository;

namespace uvw_iot::util {
class Site;
}
using uvw_iot::util::Site;

class StrategyFactory {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const SymbolRepository& symbolRepository,
                                          const Site& site,
                                          const RuleEngine& rules,
                                          const ConfigRepository& cfg);
};
