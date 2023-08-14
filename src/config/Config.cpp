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

std::chrono::milliseconds Config::primaryInterval() const {
    return _primaryInterval;
}

std::chrono::milliseconds Config::secondaryInterval() const {
    return _secondaryInterval;
}

int Config::maximumSiteAmperage() const {
    return 40;
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
}

std::optional<HostConfig> Config::hostConfig(const std::string& ) const {
    return {};
}
