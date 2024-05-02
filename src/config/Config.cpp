#include "Config.h"

#include <fstream>

#include <toml.hpp>

#include <common/Logger.h>
#include <common/Rpp.h>
#include <common/Util.h>

using namespace uvw_iot;

class Config::Impl {
public:
    toml::table configTable;

    // Sane values for interval is > 2s
    // Sane values for tau is > 3s (mayber better 4s)
    publish_subject<int> thingIntervalSubject;
    int thingInterval = 10;
    publish_subject<int> tauSubject;
    int tau = 3;

    double evseAlpha = computeFactor(thingInterval, tau);
    double evseBeta = computeFactor(thingInterval, 60.0);
    double evsePhi = 0.95;

    static double computeFactor(double dT, double tau) {
        return 1 - exp(-dT/tau);
    }

    Impl() {
        thingIntervalSubject.get_observable()
            | combine_latest(tauSubject.get_observable())
            | subscribe([this](const auto& v) {
                  evseAlpha = computeFactor(std::get<0>(v), std::get<1>(v));
                  LOG_S(INFO) << "EV charging strategy alpha> " << evseAlpha;
              });

        thingIntervalSubject.get_observable()
            | map([](int dT) {
                  const auto beta = computeFactor(dT, 60.0);
                  LOG_S(INFO) << "EV charging strategy beta> " << beta;
                  return beta;
              })
            | subscribe([this](const auto& v) { evseBeta = v; });
    }
};

Config::Config(const ThingRepository& repo) :
    _repo(repo),
    _p(new Config::Impl) {

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

Config::~Config() = default;

template<class T>
T Config::valueOr(const std::string& table_, Key key, T fallback) const {
    if (!_p->configTable.contains(table_) ||
            !_p->configTable.at(table_).contains(util::toString(key))) {
        return fallback;
    }

    return toml::get_or(_p->configTable[table_][util::toString(key)], fallback);
}

void Config::persistProperty(const std::string& table, ThingPropertyKey key, const ThingPropertyValue& value) const {
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

    if (_testing) return;

    //for (const auto& e : _p->configTable) {
    //    LOG_S(INFO) << "key: " << e.first << ", value: " << e.second;
    //}

    std::ofstream configFile;
    configFile.open(_configFile);
    configFile << toml::value(_p->configTable);
}

void Config::setThingInterval(const ThingPropertyValue& seconds) const {
    _p->thingInterval = std::get<int>(seconds);
    _p->thingIntervalSubject.get_observer().on_next(_p->thingInterval);
    persistProperty("site", ThingPropertyKey::thing_interval, seconds);
}

dynamic_observable<int> Config::thingIntervalObservable() const {
    return _p->thingIntervalSubject.get_observable();
}

int Config::thingInterval() const {
    return _p->thingInterval;
}

void Config::setTimeConstant(const ThingPropertyValue& tau) const {
    _p->tau = std::get<int>(tau);
    _p->tauSubject.get_observer().on_next(_p->tau);
    persistProperty("ev_charging_strategy", ThingPropertyKey::time_constant, tau);
}

dynamic_observable<int> Config::timeConstantObservable() const {
    return _p->tauSubject.get_observable();
}

int Config::timeConstant() const {
    return _p->tau;
}

double Config::evseAlpha() const {
    return _p->evseAlpha;
}

double Config::evseBeta() const {
    return _p->evseBeta;
}

double Config::evsePhi() const {
    return _p->evsePhi; //restrict phi to a minimum of 0.8 and a maximum of 0.98.
}

void Config::parseConfigFile() {
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

    setTimeConstant(valueOr("ev_charging_strategy", Key::time_constant, (int)10));

    for (const auto& el : valueOr("site", Key::pv, toml::array())) {
        if (el.is_string())
            _pvMeters.insert(el.as_string());
    }
    _gridMeter = valueOr("site", Key::grid, std::string());
}

void Config::setPropertiesTo(const ThingPtr& thing) {
    ThingPropertyMap properties;
    auto val = value(thing->id(), ThingPropertyKey::name);
    if (val) properties[ThingPropertyKey::name] = *val;
    val = value(thing->id(), ThingPropertyKey::pinned);
    if (val) properties[ThingPropertyKey::pinned] = *val;
    val = value(thing->id(), ThingPropertyKey::offset);
    if (val) properties[ThingPropertyKey::offset] = *val;

    thing->setProperties(properties);
}

std::optional<ThingPropertyValue> Config::value(const std::string& id, ThingPropertyKey key) const {
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

// Explicit template instantiation
template std::string Config::valueOr(const std::string& table, Key key, std::string) const;
template int Config::valueOr(const std::string& table, Key key, int) const;
template bool Config::valueOr(const std::string& table, Key key, bool) const;
