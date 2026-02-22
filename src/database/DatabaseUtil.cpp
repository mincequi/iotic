#include "DatabaseUtil.h"

#include <cmath>
#include <cstddef>

DatabaseUtil::DatabaseUtil() {}

std::map<std::uint32_t, DataPoint> DatabaseUtil::toMinuteBuckets(const std::map<std::uint32_t, DataPoint>& dailyData) {
    std::map<std::uint32_t, std::vector<DataPoint>> minuteBuckets;

    for (const auto& [timestamp, dataPoint] : dailyData) {
        std::uint32_t minute = timestamp / 60; // Group by minute
        minuteBuckets[minute].push_back(dataPoint);
    }

    //return minuteBuckets;

    // Aggregate data points for each minute
    std::map<std::uint32_t, DataPoint> aggregatedData;
    for (const auto& [minute, dataPoints] : minuteBuckets) {
        // Add the average value of the data points for the minute
        if (std::holds_alternative<int16_t>(dataPoints[0])) {
            double sum = 0.0; // int32_t and division by size() gave really weird results. Using double now.
            for (const auto& dp : dataPoints) {
                sum += std::get<int16_t>(dp);
            }
            sum /= dataPoints.size();
            // Compute average while rounding to nearest integer
            aggregatedData[minute] = (int16_t)round(sum);
        } else {
            std::vector<double> sum;
            for (const auto& dp : dataPoints) {
                const auto& vec = std::get<std::vector<int16_t>>(dp);
                if (sum.empty()) {
                    sum.resize(vec.size(), 0);
                }
                for (size_t i = 0; i < vec.size(); ++i) {
                    sum[i] += vec[i];
                }
            }
            std::vector<int16_t> result(sum.size());
            for (size_t i = 0; i < sum.size(); ++i) {
                sum[i] /= dataPoints.size();
                result[i] = (int16_t)round(sum[i]);
            }

            aggregatedData[minute] = DataPoint(std::move(result));
        }
    }

    return aggregatedData;
}

std::vector<std::pair<std::uint32_t, DataPoint>> DatabaseUtil::deltaCompress(const std::map<std::uint32_t, DataPoint>& dailyData) {
    std::vector<std::pair<std::uint32_t, DataPoint>> deltaData;

    uint32_t lastTimestamp;
    DataPoint lastValue;
    bool first = true;
    for (const auto& [timestamp, dataPoint] : dailyData) {
        if (first) {
            deltaData.emplace_back(timestamp, dataPoint);
            lastTimestamp = timestamp;
            lastValue = dataPoint;
            first = false;
        } else {
            uint32_t deltaTimestamp = timestamp - lastTimestamp;
            DataPoint deltaValue;
            if (std::holds_alternative<int16_t>(dataPoint)) {
                deltaValue = static_cast<int16_t>(std::get<int16_t>(dataPoint) - std::get<int16_t>(lastValue));
            } else {
                const auto& vec = std::get<std::vector<int16_t>>(dataPoint);
                const auto& lastVec = std::get<std::vector<int16_t>>(lastValue);
                std::vector<int16_t> deltaVec(vec.size());
                for (size_t i = 0; i < vec.size(); ++i) {
                    deltaVec[i] = vec[i] - lastVec[i];
                }
                deltaValue = std::move(deltaVec);
            }
            deltaData.emplace_back(deltaTimestamp, deltaValue);
            lastTimestamp = timestamp;
            lastValue = dataPoint;
        }
    }

    return deltaData;
}
