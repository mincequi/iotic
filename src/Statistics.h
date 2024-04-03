#pragma once

#include <things/sunspec/SunSpecModel.h>
#include <things/sunspec/SunSpecStatsModel.h>

namespace sunspec {
class SunSpecThing;
}

class Statistics {
public:
    explicit Statistics();

    void reset();

    void feedModel(const sunspec::SunSpecThing& thing, const sunspec::Model& model);

    void statsChanged(const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model);

private:
    std::map<std::pair<std::string, uint16_t>, sunspec::StatsModel> _models;
};
