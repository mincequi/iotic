/*
 * Copyright (C) 2021 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std::chrono_literals;

#define cfg Config::instance()

class HostConfig {
public:
    std::string host;
    uint16_t port;
};

struct RuleConfig {
    std::string name;
    std::string when;
    std::string then;
};

namespace toml {
class Table;
}

class Config {
public:
    // These are persistent thing properties
    enum class KeyMutable {
        name,
        offset
    };

    enum class Key {
        name,
        offset,
        on,
        off,
        debounce
    };

    static Config* instance();
    virtual ~Config();

    template<class T>
    T valueOr(const std::string& table, Key key, T fallback = {}) const;
    template<class T>
    void setValue(const std::string& table, KeyMutable key, T);

    const std::set<std::string>& pvMeters() const;
    const std::string& gridMeter() const;

    std::chrono::milliseconds primaryInterval() const;
    std::chrono::milliseconds secondaryInterval() const;

    const std::vector<RuleConfig>& rules() const;

private:
    Config();

    void parse();

    std::optional<HostConfig> hostConfig(const std::string& ) const;

private:
    static Config* _instance;

    std::string _configFile;
    std::shared_ptr<toml::Table> _configTable;

    std::set<std::string> _pvMeters;
    std::string _gridMeter;

    std::chrono::milliseconds _primaryInterval = 5000ms;
    std::chrono::milliseconds _secondaryInterval = 5000ms;

    std::vector<RuleConfig> _rules;
};
