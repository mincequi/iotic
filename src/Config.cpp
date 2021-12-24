#include "Config.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.h>

Config::Config(const std::string& configFile) : m_configFile(configFile) {
}

void Config::parse() {
    auto result = toml::parse_file(m_configFile);
    if (!result) {
        return;
    }
}

std::optional<HostConfig> Config::hostConfig(const std::string& ) const {
    return {};
}
