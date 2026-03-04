#pragma once

#include <chrono>
#include <string_view>

#include <uvw_iot/ThingProperty.h>

class Database;

class ArchiveDayUseCase {
public:
    ArchiveDayUseCase(Database& database) : _database(database) {}

    void operator()(std::string_view thingId,
                    uvw_iot::ThingPropertyKey property,
                    std::chrono::year_month_day day,
                    std::chrono::minutes resolutions);

private:
    Database& _database;
};
