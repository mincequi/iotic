#include "AppDatabase.h"

#include <iostream>

AppDatabase::AppDatabase(const std::string& map) :
    _map(map),
    _thingRepository(),
    _database(_thingRepository) {

    for (const auto& kv : _database.maps("elgris_smart_meter_1900042748.power")) {
        std::cout << "map> " << kv.first << " size: " << kv.second << std::endl;
    }
}
