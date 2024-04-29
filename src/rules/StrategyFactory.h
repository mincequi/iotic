#pragma once

#include <uvw_iot/common/ThingRepository.h>

using uvw_iot::common::ThingPtr;
using uvw_iot::common::ThingRepository;

class Config;
class RulesEngine;
class Site;
class Strategy;

class StrategyFactory {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const Site& site,
                                          const RulesEngine& rules,
                                          const Config& cfg);
};
