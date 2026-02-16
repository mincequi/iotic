#pragma once

#include <chrono>

namespace uvw_iot::util {
class Site;
}

class ConfigRepository;
class StrategyRepository;
class SymbolRepository;

class PowerManager {
public:
    PowerManager(const StrategyRepository& strategyRepository,
                 const SymbolRepository& symbolRepository,
                 const uvw_iot::util::Site& site,
                 const ConfigRepository& cfg);

private:
    int _lastStepDownTs = 0;
    int _lastStepUpTs = 0;
};
