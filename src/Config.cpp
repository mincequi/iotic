#include "Config.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.h>

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
    auto result = toml::parse_file(m_configFile);
    if (!result) {
        return;
    }

    _primaryInterval = std::chrono::milliseconds(result.table()["general"]["primary_interval"].value_or(700));
    _secondaryInterval = std::chrono::milliseconds(result.table()["general"]["secondary_interval"].value_or(1100));
}

std::optional<HostConfig> Config::hostConfig(const std::string& ) const {
    return {};
}
