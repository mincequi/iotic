#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>

#include <uvw_iot/ThingRepository.h>

#include <database/Database.h>
#include <database/DatabaseUtil.h>

using namespace std::chrono;

TEST_CASE("multi downsample test", "[cpp]") {
    std::vector<double> ratios;
    for (int i = 0; i < 1; ++i) {
        std::srand(std::time(nullptr));
        std::map<uint32_t, Datapoint> data;
        // Add data points for 8 hours, with 1 data point per 5 seconds.
        // It shall be a half sine wave with 5000 watts peak, which is 500 decaWatts.
        // Also add some noise of +/- 150 decaWatts.
        for (uint32_t t = 0; t < 8 * 3600; t += 5) {
            double sineValue = sin((double)t / (8 * 3600) * M_PI);
            int16_t value = (int16_t)round(sineValue * 500); // Scale to decaWatts
            value += (std::rand() % value); // Add noise
            data[t] = { value };
        }

        auto minuteBuckets = DatabaseUtil::downsample(data, 60);
        auto fiveMinBuckets = DatabaseUtil::downsample(minuteBuckets, 5);
        //for (const auto& [hour, value] : fiveMinBuckets) {
        //    std::cout << "Hour: " << hour << ", Value: " << std::get<int16_t>(value) << std::endl;
        //}
        std::cout << std::endl;

        auto fiveMinBuckets2 = DatabaseUtil::downsample(data, 300);
        //for (const auto& [hour, value] : fiveMinBuckets2) {
        //    std::cout << "Hour: " << hour << ", Value: " << std::get<int16_t>(value) << std::endl;
        //}

        REQUIRE(fiveMinBuckets.size() == 96);
        REQUIRE(fiveMinBuckets2.size() == 96);
        for (int i = 0; i < 96; ++i) {
            auto [timestamp1, value1] = *std::next(fiveMinBuckets.begin(), i);
            auto [timestamp2, value2] = *std::next(fiveMinBuckets2.begin(), i);
            REQUIRE(timestamp1 == timestamp2);
            int16_t v1 = value1[0];
            int16_t v2 = value2[0];
            REQUIRE(abs(v1 - v2) < 2);
            ratios.push_back((double)std::max(v1, v2) / (double)std::min(v1, v2));
            if (abs(v1 - v2) == 1) {
                REQUIRE(((double)std::max(v1, v2) / (double)std::min(v1, v2)) < 1.029);
            }
        }
    }

    double averageRatio = std::accumulate(ratios.begin(), ratios.end(), 0.0) / ratios.size();
    REQUIRE(averageRatio < 1.001);
}

TEST_CASE("cbor encode and decode test", "[cpp]") {
    Dataset data;
    // Add data points for 8 hours, with 1 data point per 5 seconds.
    // It shall be a half sine wave with 5000 watts peak, which is 500 decaWatts.
    // Also add some noise of +/- 150 decaWatts.
    for (uint32_t t = 0; t < 8 * 3600; t += 5) {
        double sineValue = sin((double)t / (8 * 3600) * M_PI);
        Datapoint vec;
        vec.push_back((int16_t)round(sineValue * 500)); // Scale to decaWatts
        vec.push_back((int16_t)round(sineValue * 350)); // Scale to decaWatts
        data[t] = vec;
    }

    // delta compress actually changes the data, because it leaves out samples for zero
    // So, decompressed deltas serve as starting point for test
    auto deltaData = DatabaseUtil::deltaCompress(data);
    data = DatabaseUtil::deltaDecompress(deltaData);

    auto cborData = DatabaseUtil::cborEncode(deltaData);
    auto deltaResult = DatabaseUtil::cborDecode(cborData);
    auto resultData = DatabaseUtil::deltaDecompress(deltaResult);

    REQUIRE(data.size() == resultData.size());
    for (int i = 0; i < deltaData.size(); ++i) {
        auto [timestamp1, value1] = *std::next(data.begin(), i);
        auto [timestamp2, value2] = *std::next(resultData.begin(), i);
        REQUIRE(timestamp1 == timestamp2);
        REQUIRE(value1 == value2);
    }
}

TEST_CASE("mdbx file growth test", "[cpp]") {
    Database::Config config { .dbFile = "test.mdbx" };
    Database db(config, {});

    for (uint32_t t = 0; t < 7 * 24 * 3600; t += 5) {
        double sineValue = sin((double)t / (12 * 3600) * M_PI);
        Datapoint vec;
        vec.push_back((int16_t)round(sineValue * 500)); // Scale to decaWatts
        vec.push_back((int16_t)round(sineValue * 350)); // Scale to decaWatts

        db.putDatapoint("testThing", uvw_iot::ThingPropertyKey::power, t, vec);
    }


    for (unsigned i = 0; i < 7; ++i) {
        year_month_day ymd {
            year{1970},
            month{1},
            day{i+1}
        };
        auto data = db.rawData("testThing", uvw_iot::ThingPropertyKey::power, ymd);
        //REQUIRE(data.size() > 0);
        db.eraseRawData("testThing", uvw_iot::ThingPropertyKey::power, ymd);
        auto dataAfterErase = db.rawData("testThing", uvw_iot::ThingPropertyKey::power, ymd);
        REQUIRE(dataAfterErase.empty());
    }

    REQUIRE(db.mapSize("testThing", uvw_iot::ThingPropertyKey::power) == 0);
}

TEST_CASE("TestCaseName") {
    Database::Config config { .dbFile = "test.mdbx" };
    Database db(config, /*thingRepository=*/{});

    REQUIRE(db.archivedData("someThing",
                            uvw_iot::ThingPropertyKey::dcPower,
                            year{2024}/June/1,
                            minutes{5}) == std::string_view{});
}

