#include "Config.h"

#include <fstream>
#include <iostream>

#define TOML_EXCEPTIONS 0
#include <toml.hpp>

#include <common/Logger.h>
#include <common/Util.h>
#include <things/ThingsRepository.h>

struct Config::Impl {
    toml::parse_result configTable;
};

Config::Config(const ThingsRepository& thingsRepository) :
    _thingsRepository(thingsRepository),
    _p(new Config::Impl) {
    parse();
    _thingsRepository.thingAdded().subscribe([this](const auto& thing) {
        thing->properties().subscribe([this, &thing](const std::map<Property, Value>& prop) {
            for (const auto& kv : prop) {
                switch (kv.first) {
                case Property::name:
                case Property::pinned:
                    setValue(thing->id(), kv.first, kv.second);
                    break;
                default:
                    break;
                }
            }
        });
    });
}

Config::~Config() {
}

template<class T>
T Config::valueOr(const std::string& table_, Key key, T fallback) const {
    return _p->configTable[table_][util::toString(key)].value_or(fallback);
}
// Explicit template instantiation
template std::string Config::valueOr(const std::string& table, Key key, std::string) const;
template int Config::valueOr(const std::string& table, Key key, int) const;
template bool Config::valueOr(const std::string& table, Key key, bool) const;

void Config::setValue(const std::string& table, Property key, const Value& value) {
    if (!_p->configTable.table().contains(table)) {
    _p->configTable.table().insert_or_assign(table, toml::table{});
    }
    auto section = _p->configTable.table().at(table).as_table();
    std::visit([&](const auto& arg){ section->insert_or_assign(util::toString(key), arg); }, value);

    std::ofstream configFile;
    configFile.open(_configFile);
    configFile << _p->configTable;
}

void Config::parse() {
    _p->configTable = toml::parse_file(_configFile);
    if (!_p->configTable) {
        LOG_S(FATAL) << "Error parsing config file: " << _configFile;
        return;
    }

    _discoveryInterval = std::chrono::milliseconds(_p->configTable["general"]["discovery_interval"].value_or(60000));
    LOG_S(INFO) << "discovery_interval: " << _thingInterval.count() << "ms";
    _thingInterval = std::chrono::milliseconds(_p->configTable["general"]["thing_interval"].value_or(10000));
    LOG_S(INFO) << "thing_interval: " << _thingInterval.count() << "ms";

    if (auto pvMeters = _p->configTable["site"]["pv"].as_array()) {
        pvMeters->for_each([this](auto& el) {
            if constexpr (toml::is_string<decltype(el)>)
                    _pvMeters.insert(el.get());
        });
    }

    _gridMeter = _p->configTable["site"]["grid"].value_or("");
}
