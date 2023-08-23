#include "Config.h"

#define TOML_EXCEPTIONS 0
#include <tomlcpp.hpp>

Config* Config::_instance = nullptr;

Config* Config::instance() {
    if (_instance == nullptr) {
        _instance = new Config;
        _instance->parse();
    }
    return _instance;
}

Config::Config() :
    m_configFile("/etc/elsewhere.conf") {
}

Config::~Config() {
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

void Config::parse() {
    auto result = toml::parseFile(m_configFile);
    if (!result.table) {
        return;
    }
    auto general = result.table->getTable("general");
    if (!general) {
        return;
    }
    {
        auto [ok, interval] = general->getInt("primary_interval");
        if (!ok) interval = 10000;
        _primaryInterval = std::chrono::milliseconds(interval);
    }
    {
        auto [ok, interval] = general->getInt("secondary_interval");
        if (!ok) interval = 10000;
        _secondaryInterval = std::chrono::milliseconds(interval);
    }

    auto site = result.table->getTable("site");
    if (!site) {
        return;
    }

    auto pv = site->getArray("pv");
    if (pv) {
        auto pvMeters_ = pv->getStringVector();
        if (pvMeters_)
            _pvMeters = { pvMeters_->begin(), pvMeters_->end() };
    }

    {
        auto [ok, gridMeter_] = site->getString("grid");
        if (ok)
            _gridMeter = gridMeter_;
    }
}

std::optional<HostConfig> Config::hostConfig(const std::string& ) const {
    return {};
}
