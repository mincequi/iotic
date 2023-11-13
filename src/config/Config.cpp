#include "Config.h"

#include <fstream>
#include <iostream>

#define TOML_EXCEPTIONS 0
#include <toml.hpp>

#include <common/Logger.h>
#include <common/Rpp.h>
#include <common/Util.h>
#include <things/ThingsRepository.h>

struct Config::Impl {
    toml::parse_result configTable;
    behavior_subject<std::chrono::milliseconds> thingInterval = 10000ms;
    behavior_subject<double> tau = 3.0;

    double evseAlpha = computeFactor(thingInterval.get_value().count()/1000.0, tau.get_value());
    double evseBeta = computeFactor(thingInterval.get_value().count()/1000.0, 60.0);
    double evsePhi = 0.95;

    static double computeFactor(double dT, double tau) {
        return 1 - exp(-dT/tau);
    }

    Impl() {
        thingInterval.get_observable().combine_latest([](const auto& dT, double tau) {
            const auto alpha = computeFactor(dT.count()/1000.0, tau);
            LOG_S(INFO) << "EV charging strategy alpha> " << alpha;
            return alpha;
        }, tau.get_observable()).subscribe([this](const auto& v) { evseAlpha = v; });

        thingInterval.get_observable().map([](const auto& dT) {
            const auto beta = computeFactor(dT.count()/1000.0, 60.0);
            LOG_S(INFO) << "EV charging strategy beta> " << beta;
            return beta;
        }).subscribe([this](const auto& v) { evseBeta = v; });
    }
};

Config* Config::instance() {
    if (!_instance) _instance = new Config();
    return _instance;
}

Config::Config() :
    _p(new Config::Impl) {
    parse();
    repo->thingAdded().subscribe([this](const auto& thing) {
        thing->properties().subscribe([this, &thing](const std::map<Property, Value>& props) {
            for (const auto& kv : props) {
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

template<class T>
T Config::valueOr(const std::string& table_, Key key, T fallback) const {
    return _p->configTable[table_][util::toString(key)].value_or(fallback);
}

void Config::setValue(const std::string& table, Property key, const Value& value) {
    if (!_p->configTable.table().contains(table)) {
        _p->configTable.table().insert_or_assign(table, toml::table{});
    }
    auto section = _p->configTable.table().at(table).as_table();
    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::array<double, 3>>) {}
        else {
            section->insert_or_assign(util::toString(key), arg);
        }
    }, value);

    if (_testing) return;

    std::ofstream configFile;
    configFile.open(_configFile);
    configFile << _p->configTable;
}

void Config::setThingInterval(int seconds) {
    _p->thingInterval.get_subscriber().on_next(std::chrono::milliseconds(seconds * 1000));
    setValue("site", Property::thing_interval, seconds * 1000);
}

std::chrono::milliseconds Config::thingInterval() const {
    return _p->thingInterval.get_value();
}

void Config::setEvseTau(double tau) {
    _p->tau.get_subscriber().on_next(tau);
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

void Config::parse() {
    _p->configTable = toml::parse_file(_configFile);
    if (!_p->configTable) {
        LOG_S(FATAL) << "Error parsing config file: " << _configFile;
        return;
    }

    _discoveryInterval = std::chrono::milliseconds(_p->configTable["site"]["discovery_interval"].value_or(60000));
    LOG_S(INFO) << "discovery_interval: " << _discoveryInterval.count() << "ms";
    _p->thingInterval.get_subscriber().on_next(std::chrono::milliseconds(_p->configTable["site"]["thing_interval"].value_or(10000)));
    LOG_S(INFO) << "thing_interval: " << thingInterval().count() << "ms";

    if (auto pvMeters = _p->configTable["site"]["pv"].as_array()) {
        pvMeters->for_each([this](auto& el) {
            if constexpr (toml::is_string<decltype(el)>)
                    _pvMeters.insert(el.get());
        });
    }

    _gridMeter = _p->configTable["site"]["grid"].value_or("");
}

// Explicit template instantiation
template std::string Config::valueOr(const std::string& table, Key key, std::string) const;
template int Config::valueOr(const std::string& table, Key key, int) const;
template bool Config::valueOr(const std::string& table, Key key, bool) const;
