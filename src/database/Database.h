#pragma once

#include <chrono>
#include <memory>

#include <uvw_iot/ThingProperty.h>

#include "DatabaseTypes.h"

namespace uvw_iot {
class ThingRepository;
}

class Database {
public:
    Database(const uvw_iot::ThingRepository& thingRepository);
    ~Database();   // IMPORTANT: Because of the pimpl idiom, we need to define destructor in .cpp file, otherwise it will cause linker error

    std::string info() const;
    std::string stat() const;

    bool hasMap(std::string_view thingId, uvw_iot::ThingPropertyKey key) const;

    //std::vector<int16_t> yearlyData(const std::string& thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year& year) const;
    //std::vector<int16_t> monthlyData(const std::string& thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month& ym) const;
    Dataset rawData(std::string_view thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month_day& day) const;
    void putDatapoint(const std::string& thingId, uvw_iot::ThingPropertyKey property, uint32_t timestamp, const Datapoint& datapoint);

    std::string_view archivedData(std::string_view thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month_day& day);
    void putArchivedData(std::string_view thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month_day& day, std::string_view data);

private:
    std::unique_ptr<class DatabasePrivate> const d;
};
