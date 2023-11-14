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
    behavior_subject<int> thingInterval = 10;
    behavior_subject<int> tau = 3;

    double evseAlpha = computeFactor(thingInterval.get_value(), tau.get_value());
    double evseBeta = computeFactor(thingInterval.get_value(), 60.0);
    double evsePhi = 0.95;

    static double computeFactor(double dT, double tau) {
        return 1 - exp(-dT/tau);
    }

    Impl() {
        thingInterval.get_observable().combine_latest([](double dT, double tau) {
            const auto alpha = computeFactor(dT, tau);
            LOG_S(INFO) << "EV charging strategy alpha> " << alpha;
            return alpha;
        }, tau.get_observable()).subscribe([this](const auto& v) { evseAlpha = v; });

        thingInterval.get_observable().map([](double dT) {
            const auto beta = computeFactor(dT, 60.0);
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
    _p->thingInterval.get_subscriber().on_next(seconds);
    setValue("site", Property::thing_interval, seconds);
}

int Config::thingInterval() const {
    return _p->thingInterval.get_value();
}

void Config::setTimeConstant(const Value& tau) {
    _p->tau.get_subscriber().on_next(std::get<double>(tau));
    setValue("ev_charging_strategy", Property::time_constant, (int)(std::get<double>(tau)));
}

const behavior_subject<int>& Config::timeConstant() const {
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

void Config::parse() {
    _p->configTable = toml::parse_file(_configFile);
    if (!_p->configTable) {
        LOG_S(FATAL) << "Error parsing config file: " << _configFile;
        return;
    }

    _discoveryInterval = _p->configTable["site"]["discovery_interval"].value_or(60);
    LOG_S(INFO) << "discovery_interval: " << _discoveryInterval << "s";
    _p->thingInterval.get_subscriber().on_next(_p->configTable["site"]["thing_interval"].value_or(10));
    LOG_S(INFO) << "thing_interval: " << thingInterval() << "s";
    _p->tau.get_subscriber().on_next(_p->configTable["ev_charging_strategy"]["time_constant"].value_or(10));

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
