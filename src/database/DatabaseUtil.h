#pragma once

#include <map>
#include <utility>
#include <vector>

#include "DatabaseTypes.h"

class DatabaseUtil {
public:
    DatabaseUtil();

    static std::map<std::uint32_t, DataPoint> toMinuteBuckets(
        const std::map<std::uint32_t, DataPoint>& dailyData,
        int minutes = 1,
        int watts = 10);
    static std::vector<std::pair<std::uint32_t, DataPoint>> deltaCompress(const std::map<std::uint32_t, DataPoint>& dailyData);
};
