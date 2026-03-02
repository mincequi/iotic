#include "DatabaseUtil.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

#include <decoder.h>
#include <encoder.h>
#include <input.h>
#include <output_dynamic.h>

Dataset DatabaseUtil::downsample(const Dataset& samples, int timeDivisor, int watts) {
    std::map<std::uint32_t, std::vector<Datapoint>> buckets;

    for (const auto& [timestamp, dataPoint] : samples) {
        std::uint32_t bucketTimestamp = timestamp / timeDivisor;
        buckets[bucketTimestamp].push_back(dataPoint);
    }

    // Aggregate data points for each minute
    std::map<std::uint32_t, Datapoint> aggregates;
    for (const auto& [timestamp, dataPoints] : buckets) {
        // Add the average value of the data points for the minute
        std::vector<double> sum;
        for (const auto& dp : dataPoints) {
            if (sum.empty()) {
                sum.resize(dp.size(), 0);
            }
            for (size_t i = 0; i < dp.size(); ++i) {
                sum[i] += dp[i];
            }
        }
        std::vector<int16_t> result(sum.size());
        for (size_t i = 0; i < sum.size(); ++i) {
            sum[i] /= dataPoints.size();
            sum[i] /= (watts/10); // convert to decaWatts
            result[i] = (int16_t)round(sum[i]);
        }

        aggregates[timestamp] = Datapoint(std::move(result));
    }

    return aggregates;
}

DeltaDataset DatabaseUtil::deltaCompress(const Dataset& dataset) {
    DeltaDataset deltaDataset;

    uint32_t lastTimestamp;
    Datapoint lastDatapoint;
    bool first = true;
    for (const auto& [timestamp, datapoint] : dataset) {
        if (first) {
            deltaDataset.emplace_back(timestamp, datapoint);
            lastTimestamp = timestamp;
            lastDatapoint = datapoint;
            first = false;
        } else {
            uint32_t deltaTimestamp = timestamp - lastTimestamp;
            Datapoint deltaDatapoint(datapoint.size());
            for (size_t i = 0; i < datapoint.size(); ++i) {
                deltaDatapoint[i] = datapoint[i] - lastDatapoint[i];
            }
            if (std::all_of(deltaDatapoint.begin(), deltaDatapoint.end(), [](int16_t v) { return v == 0; })) {
                continue; // skip if value hasn't changed
            }

            deltaDataset.emplace_back(deltaTimestamp, deltaDatapoint);
            lastTimestamp = timestamp;
            lastDatapoint = datapoint;
        }
    }

    return deltaDataset;
}

Dataset DatabaseUtil::deltaDecompress(const DeltaDataset& deltaData) {
    Dataset samples;

    uint32_t currentTimestamp = 0;
    Datapoint currentValue;
    bool first = true;
    for (const auto& [deltaTimestamp, deltaValue] : deltaData) {
        currentTimestamp += deltaTimestamp;
        if (first) {
            currentValue = deltaValue;
            first = false;
        } else {
            Datapoint value;
            std::vector<int16_t> vec(deltaValue.size());
            for (size_t i = 0; i < deltaValue.size(); ++i) {
                vec[i] = currentValue[i] + deltaValue[i];
            }
            currentValue = std::move(vec);
        }

        samples[currentTimestamp] = currentValue;
    }

    return samples;
}

std::vector<uint8_t> DatabaseUtil::cborEncode(const DeltaDataset& data) {
    cbor::output_dynamic out;
    cbor::encoder encoder(out);

    if (data.empty()) {
        return {};
    }

    // Write an array of timestamps
    encoder.write_array(data.size());
    for (const auto& [timestamp, _] : data) {
        encoder.write_int(timestamp);
    }

    // Write an array for each value index
    for (int i = 0; i < data.front().second.size(); ++i) {
        encoder.write_array(data.size());
        for (const auto& [_, dataPoint] : data) {
            encoder.write_int(dataPoint[i]);
        }
    }

    return std::vector<uint8_t>(out.data(), out.data() + out.size());
}

DeltaDataset DatabaseUtil::cborDecode(const std::vector<uint8_t>& cborData) {
    std::vector<uint32_t> timestamps;
    std::vector<std::vector<int16_t>> values;

    class Listener : public cbor::listener {
    public:
        Listener(std::vector<uint32_t>& timestamps,
                 std::vector<std::vector<int16_t>>& values) :
        _timestamps(timestamps),
        _values(values) {}

        void on_integer(int value) {
            switch (_state) {
            case Timestamp:
                _timestamps.push_back(value);
                return;
            case Value:
                _values.back().push_back(value);
                return;
            }
        }
        void on_float32(float value) {}
        void on_double(double value) {}
        void on_bytes(unsigned char *data, int size) {}
        void on_string(std::string &str) {}
        void on_array(int size) {
            switch (_state) {
            case Initial:
                _timestamps.reserve(size);
                _state = Timestamp;
                return;
            case Timestamp:
                _values.emplace_back();
                _state = Value;
                return;
            case Value:
                _values.emplace_back();
                return;
            }

        }
        void on_map(int size) {}
        void on_tag(unsigned int tag) {}
        void on_special(unsigned int code) {}
        void on_bool(bool) {}
        void on_null() {}
        void on_undefined() {}
        void on_error(const char *error) {}

        std::vector<uint32_t>& _timestamps;
        std::vector<std::vector<int16_t>>& _values;

        enum State {
            Initial,
            Timestamp,
            Value
        } _state = Initial;
        int _remainingVectorElements = 0;
    };

    Listener listener(timestamps, values);
    cbor::input input((void*)cborData.data(), cborData.size());
    cbor::decoder decoder(input, listener);
    decoder.run();

    DeltaDataset out;
    out.reserve(timestamps.size());
    for (size_t i = 0; i < timestamps.size(); ++i) {
        std::vector<int16_t> values_;
        for (size_t j = 0; j < values.size(); ++j) {
            values_.push_back(values[j][i]);
        }
        out.emplace_back(timestamps[i], Datapoint(std::move(values_)));
    }
    return out;
}
