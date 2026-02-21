#pragma once

#include <chrono>
#include <map>
#include <memory>

#include <uvw_iot/ThingProperty.h>

namespace uvw_iot {
class ThingRepository;
}

class Database {
public:
    using DataPoint = std::variant<int16_t, std::vector<int16_t>>;

    Database(const uvw_iot::ThingRepository& thingRepository);
    ~Database();   // IMPORTANT: Because of the pimpl idiom, we need to define destructor in .cpp file, otherwise it will cause linker error

    std::string info() const;
    std::string stat() const;

    bool hasMap(std::string_view thingId, uvw_iot::ThingPropertyKey key) const;

    //std::vector<int16_t> yearlyData(const std::string& thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year& year) const;
    //std::vector<int16_t> monthlyData(const std::string& thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month& ym) const;
    std::map<uint32_t, DataPoint> dailyData(std::string_view thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month_day& ymd) const;

private:
    std::unique_ptr<class DatabasePrivate> const d;
};
