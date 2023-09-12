#include "Config.h"

#define TOML_EXCEPTIONS 0
#include <tomlcpp.hpp>

#include <common/Logger.h>
#include <common/Util.h>

Config* Config::_instance = nullptr;

Config* Config::instance() {
    if (_instance == nullptr) {
        _instance = new Config;
        _instance->parse();
    }
    return _instance;
}

Config::Config() :
    _configFile("/etc/elsewhere.conf") {
}

Config::~Config() {
}

template<class T>
T Config::valueOr(const std::string& table_, Key key, T fallback) const {
    auto table = _configTable->getTable(table_);
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

const std::set<std::string>& Config::pvMeters() const {
    return _pvMeters;
}

const std::string& Config::gridMeter() const {
    return _gridMeter;
}

std::chrono::milliseconds Config::primaryInterval() const {
    return _primaryInterval;
}

std::chrono::milliseconds Config::secondaryInterval() const {
    return _secondaryInterval;
}

const std::vector<RuleConfig>& Config::rules() const {
    return _rules;
}

void Config::parse() {
    auto result = toml::parseFile(_configFile);
    if (!result.table) {
        LOG_S(FATAL) << "Error parsing config file: " << _configFile;
        return;
    }
    _configTable = result.table;
    auto general = _configTable->getTable("general");
    if (general) {
        auto [ok1, interval1] = general->getInt("primary_interval");
        if (!ok1) { interval1 = 10000; }
        _primaryInterval = std::chrono::milliseconds(interval1);
        auto [ok2, interval2] = general->getInt("secondary_interval");
        if (!ok2) { interval2 = 10000; }
        _secondaryInterval = std::chrono::milliseconds(interval2);
    };

    auto site = _configTable->getTable("site");
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

    /*
    auto rules = _configTable->getTable("rules");
    for (const auto& rule : rules->keys()) {
        auto table = rules->getTable(rule);
        auto [okWhen, when] = table->getString("when");
        auto [okThen, then] = table->getString("then");
        _rules.push_back( { rule, when, then } );
    }
    */
}

std::optional<HostConfig> Config::hostConfig(const std::string& ) const {
    return {};
}
