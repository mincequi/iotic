#include "ConfigRepository.h"

#include <fstream>

#include <toml.hpp>

#include <common/Logger.h>
#include <common/Rpp.h>
#include <common/Util.h>

using namespace uvw_iot;

class ConfigRepository::Impl {
public:
    toml::table configTable;

    // Sane values for interval is > 2s
    // Sane values for tau is > 3s (mayber better 4s)
    publish_subject<int> thingIntervalSubject;
    int thingInterval = 10;
};

ConfigRepository::ConfigRepository(const ThingRepository& repo) :
    _repo(repo),
    _p(new ConfigRepository::Impl) {

    parseConfigFile();

    _repo.thingAdded().subscribe([this](ThingPtr thing) {
        // Apply properties to thing that got persisted before
        setPropertiesTo(thing);

        thing->propertiesObservable().subscribe([this, thing](const auto& props) {
            // Persist properties that got applied from thing
            for (const auto& kv : props) {
                switch (kv.first) {
                case ThingPropertyKey::name:
                case ThingPropertyKey::pinned:
                case ThingPropertyKey::offset:
                    persistProperty(thing->id(), kv.first, kv.second);
                    break;
                default:
                    break;
                }
            }
        });
    });
}

ConfigRepository::~ConfigRepository() = default;

template<class T>
T ConfigRepository::valueOr(const std::string& table_, Key key, T fallback) const {
    if (!_p->configTable.contains(table_) ||
            !_p->configTable.at(table_).contains(util::toString(key))) {
        return fallback;
    }

    return toml::get_or(_p->configTable[table_][util::toString(key)], fallback);
}

template<class T>
T ConfigRepository::valueOr(Key key, T fallback) const {
    return valueOr("site", key, fallback);
}

void ConfigRepository::setThingInterval(const ThingPropertyValue& seconds) const {
    _p->thingInterval = std::get<int>(seconds);
    _p->thingIntervalSubject.get_observer().on_next(_p->thingInterval);
    persistProperty("site", ThingPropertyKey::thing_interval, seconds);
}

dynamic_observable<int> ConfigRepository::thingIntervalObservable() const {
    return _p->thingIntervalSubject.get_observable();
}

int ConfigRepository::thingInterval() const {
    return _p->thingInterval;
}

int ConfigRepository::hysteresisFor(int power) const {
    const int absolute = valueOr(Key::hysteresis_absolute, 200);
    const int relative = round(power * (100 + valueOr(Key::hysteresis_percent, 10)) / 100.0);

    return std::max(absolute, relative);
}

void ConfigRepository::parseConfigFile() {
    std::ifstream configFile(_configFile);
    if (configFile.good()) {
        _p->configTable = toml::parse(_configFile).as_table();
    } else {
        LOG_S(WARNING) << "Error parsing config file: " << _configFile;
        _p->configTable = toml::table();
    }

    _discoveryInterval = valueOr("site", Key::discovery_interval, 60);
    LOG_S(INFO) << "discovery_interval: " << _discoveryInterval << "s";

    setThingInterval(valueOr("site", Key::thing_interval, 10));
    LOG_S(INFO) << "thing_interval: " << thingInterval() << "s";

    for (const auto& el : valueOr("site", Key::pv, toml::array())) {
        if (el.is_string())
            _pvMeters.insert(el.as_string());
    }
    _gridMeter = valueOr("site", Key::grid, std::string());
}

void ConfigRepository::setPropertiesTo(const ThingPtr& thing) {
    ThingPropertyMap properties;
    auto val = value(thing->id(), ThingPropertyKey::name);
    if (val) properties[ThingPropertyKey::name] = *val;
    val = value(thing->id(), ThingPropertyKey::pinned);
    if (val) properties[ThingPropertyKey::pinned] = *val;
    val = value(thing->id(), ThingPropertyKey::offset);
    if (val) properties[ThingPropertyKey::offset] = *val;

    thing->setProperties(properties);
}

std::optional<ThingPropertyValue> ConfigRepository::value(const std::string& id, ThingPropertyKey key) const {
    if (!_p->configTable.contains(id) || !_p->configTable.at(id).contains(util::toString(key))) {
        return {};
    }

    auto t = _p->configTable[id];
    const toml::value v = t[util::toString(key)];
    switch (v.type()) {
        case toml::value_t::boolean:
            return v.as_boolean();
        case toml::value_t::integer:
            return (int)v.as_integer();
        case toml::value_t::string:
            return v.as_string();
        default:
            return {};
    }
}

void ConfigRepository::persistProperty(const std::string& table, ThingPropertyKey key, const ThingPropertyValue& value) const {
    if (!_p->configTable.contains(table))
        _p->configTable[table] = toml::table();
    auto& section = _p->configTable[table].as_table();

    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::array<int, 3>>)
        {}
        else
            section.insert_or_assign(util::toString(key), arg);
    }, value);

    std::ofstream configFile;
    configFile.open(_configFile);
    configFile << toml::value(_p->configTable);
}

// Explicit template instantiation
template std::string ConfigRepository::valueOr(const std::string& table, Key key, std::string) const;
template int ConfigRepository::valueOr(const std::string& table, Key key, int) const;
template bool ConfigRepository::valueOr(const std::string& table, Key key, bool) const;
template double ConfigRepository::valueOr(const std::string& table, Key key, double) const;

template std::string ConfigRepository::valueOr(Key key, std::string) const;
template int ConfigRepository::valueOr(Key key, int) const;
template bool ConfigRepository::valueOr(Key key, bool) const;
template double ConfigRepository::valueOr(Key key, double) const;
