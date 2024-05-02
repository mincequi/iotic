#pragma once

#include <uvw_iot/ThingRepository.h>

using uvw_iot::ThingPtr;
using uvw_iot::ThingRepository;

class Config;
class RulesEngine;
class Strategy;

namespace uvw_iot {
class Site;
}
using uvw_iot::Site;

class StrategyFactory {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const Site& site,
                                          const RulesEngine& rules,
                                          const Config& cfg);
};
