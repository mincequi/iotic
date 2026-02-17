#pragma once

#include <map>
#include <string>
#include <vector>

namespace uvw_iot {
class ThingRepository;
}

class StatsDaily {
public:
    StatsDaily(const uvw_iot::ThingRepository& thingRepository);

private:
    struct ThingStats {
        int currentDay = 0;
        int timestamp = 0;
        std::vector<int> energyDelivered;
    };

    std::map<std::string, ThingStats> _stats;
};
