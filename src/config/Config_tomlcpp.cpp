#include "Config.h"

#define TOML_EXCEPTIONS 0
#include <tomlcpp.hpp>

#include <common/Logger.h>
#include <common/Util.h>

struct Config::Impl {
    std::shared_ptr<toml::Table> _configTable;
};

template<class T>
T Config::valueOr(const std::string& table_, Key key, T fallback) const {
    auto table = _p->_configTable->getTable(table_);
    if (!table) return fallback;
    if constexpr (std::is_same_v<T, std::string>) {
        auto p = table->getString(util::toString(key));
        if (p.first) return p.second;
    } else if constexpr (std::is_same_v<T, int>) {
        auto p = table->getInt(util::toString(key));
        if (p.first) return p.second;
    }
    return fallback;
}
// Explicit template instantiation
template std::string Config::valueOr(const std::string& table, Key key, std::string) const;
template int Config::valueOr(const std::string& table, Key key, int) const;

template<class T>
void Config::setValue(const std::string& table, KeyMutable key, T) {
    //_configFile.
}

Config::Config() :
    _p(new Config::Impl) {
}

Config::~Config() {
}

void Config::parse() {
    auto result = toml::parseFile(_configFile);
    if (!result.table) {
        LOG_S(FATAL) << "Error parsing config file: " << _configFile;
        return;
    }
    _p->_configTable = result.table;
    auto general = _p->_configTable->getTable("general");
    if (general) {
        auto [ok1, interval1] = general->getInt("primary_interval");
        if (!ok1) { interval1 = 10000; }
        LOG_S(INFO) << "interval: " << interval1 << "ms";
        _thingInterval = std::chrono::milliseconds(interval1);
    };

    auto site = _p->_configTable->getTable("site");
    if (site) {
        auto pv = site->getArray("pv");
        if (pv) {
            auto pvMeters_ = pv->getStringVector();
            if (pvMeters_)
                _pvMeters = { pvMeters_->begin(), pvMeters_->end() };
        }
        auto [ok, gridMeter_] = site->getString("grid");
        if (ok)
            _gridMeter = gridMeter_;
    }
}
