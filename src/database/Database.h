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
    struct Config {
        std::string dbFile = "/var/lib/chronobert/chronobert.mdbx";
    };

    Database(const Config& config,
             const uvw_iot::ThingRepository& thingRepository);
    // note: because of pimpl pattern, we have to define destructor in .cpp file,
    //       otherwise it won't compile because of incomplete type of unique_ptr.
    ~Database();

    std::string info() const;
    std::string stat() const;

    std::map<std::string, int> maps(const std::string& map = {}) const;
    size_t mapSize(std::string_view thingId, uvw_iot::ThingPropertyKey property) const;

    Dataset rawData(std::string_view thingId,
                    uvw_iot::ThingPropertyKey property,
                    std::chrono::year_month_day day) const;
    void putDatapoint(const std::string& thingId,
                      uvw_iot::ThingPropertyKey property,
                      uint32_t timestamp,
                      const Datapoint& datapoint);
    void eraseRawData(std::string_view thingId,
                      uvw_iot::ThingPropertyKey property,
                      std::chrono::year_month_day day);

    std::string_view archivedData(std::string_view thingId,
                                  uvw_iot::ThingPropertyKey property,
                                  std::chrono::year_month_day day,
                                  std::chrono::minutes resolution);
    void putArchivedData(std::string_view thingId,
                         uvw_iot::ThingPropertyKey property,
                         std::chrono::year_month_day day,
                         std::chrono::minutes resolution,
                         std::string_view data);

private:
    std::unique_ptr<class DatabasePrivate> const d;
};
