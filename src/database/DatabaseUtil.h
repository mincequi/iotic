#pragma once

#include <chrono>

#include "DatabaseTypes.h"

class DatabaseUtil {
public:
    static Dataset downsample(const Dataset& samples, int timeDivisor = 60, int watts = 10);

    static DeltaDataset deltaCompress(const Dataset& samples);
    static Dataset deltaDecompress(const DeltaDataset& deltaData);

    static std::vector<uint8_t> cborEncode(const DeltaDataset& data);
    static DeltaDataset cborDecode(const std::vector<uint8_t>& cborData);

    static uint32_t daySinceEpoch(const std::chrono::year_month_day& day) {
        return std::chrono::system_clock::to_time_t(std::chrono::sys_days(day)) / (24 * 60 * 60);
    }
};
