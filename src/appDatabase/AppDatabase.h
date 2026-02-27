#pragma once

#include <uvw_iot/ThingRepository.h>

#include <database/Database.h>

class AppDatabase {
public:
    AppDatabase(const std::string& map = {});

private:
    const std::string _map;
    uvw_iot::ThingRepository _thingRepository;
    Database _database;
};
