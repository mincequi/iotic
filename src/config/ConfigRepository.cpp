#include "ConfigRepository.h"

#include <fstream>

#include <toml.hpp>

#include <common/Logger.h>
#include <common/Rpp.h>
#include <common/Util.h>

using namespace uvw_iot;

class ConfigRepository::Impl {
public:
    mutable toml::table configTable;

    // Sane values for interval is > 2s
    // Sane values for tau is > 3s (mayber better 4s)
    publish_subject<int> thingIntervalSubject;
    int thingInterval = 10;

    toml::value value(const std::string& id, ThingPropertyKey key) const {
        if (!configTable.contains(id) || !configTable.at(id).contains(util::toString(key))) {
            return {};
        }

        auto t = configTable[id];
        const toml::value v = t[util::toString(key)];
        return v;
    }
};

ConfigRepository::ConfigRepository(const ThingRepository& repo) :
    _repo(repo),
    _p(new ConfigRepository::Impl) {

    parseConfigFile();

    _repo.thingAdded().subscribe([this](ThingPtr thing) {
        // Apply properties to thing that got persisted before
        setPropertiesTo(thing);

        thing->propertiesObservable().subscribe([this, thing](const auto& props) {
            props.template on<ThingPropertyKey::name>([this, thing](const auto& value) {
                persistProperty(thing->id(), ThingPropertyKey::name, value);
            }).template on<ThingPropertyKey::pinned>([this, thing](const auto& value) {
                persistProperty(thing->id(), ThingPropertyKey::pinned, value);
            }).template on<ThingPropertyKey::offset>([this, thing](const auto& value) {
                persistProperty(thing->id(), ThingPropertyKey::offset, value);
            });
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
    persistProperty("site", ThingPropertyKey::thing_interval, _p->thingInterval);
}

dynamic_observable<int> ConfigRepository::thingIntervalObservable() const {
    return _p->thingIntervalSubject.get_observable();
}

int ConfigRepository::thingInterval() const {
    return _p->thingInterval;
}

int ConfigRepository::hysteresisFor(int power) const {
    const int absolute = valueOr(Key::hysteresis_absolute, 200);
    const int relative = round(power * valueOr(Key::hysteresis_percent, 10) / 100.0);

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

    auto val = _p->value(thing->id(), ThingPropertyKey::name);
    if (val.is_string()) properties.set<ThingPropertyKey::name>(val.as_string());

    val = _p->value(thing->id(), ThingPropertyKey::pinned);
    if (val.is_boolean()) properties.set<ThingPropertyKey::pinned>(val.as_boolean());

    val = _p->value(thing->id(), ThingPropertyKey::offset);
    if (val.is_integer()) properties.set<ThingPropertyKey::offset>(val.as_integer());

    thing->setProperties(properties);
}

template<class T>
void ConfigRepository::persistProperty(const std::string& table, ThingPropertyKey key, const T& value) const {
    if (!_p->configTable.contains(table))
        _p->configTable[table] = toml::table();
    auto& section = _p->configTable[table].as_table();
    section.insert_or_assign(util::toString(key), value);

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
