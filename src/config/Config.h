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

namespace toml {
class Table;
//class table;
//namespace v3 {
//struct parse_result;
//}
}

class Config {
public:
    // These are persistent thing properties
    enum class KeyMutable {
        name,
        is_pinned
    };

    enum class Key {
        name,
        is_pinned,

        offset,
        on,
        off,
        debounce
    };

    static inline Config* instance() {
        if (_instance == nullptr) {
            _instance = new Config;
            _instance->parse();
        }
        return _instance;
    }
    virtual ~Config();

    template<class T>
    T valueOr(const std::string& table, Key key, T fallback = {}) const;
    template<class T>
    void setValue(const std::string& table, KeyMutable key, T);

    inline const std::set<std::string>& pvMeters() const {
        return _pvMeters;
    }
    inline const std::string& gridMeter() const {
        return _gridMeter;
    }
    inline std::chrono::milliseconds primaryInterval() const {
        return _primaryInterval;
    }

private:
    Config();

    void parse();

private:
    static inline Config* _instance = nullptr;

    std::string _configFile = "/etc/elsewhere.conf";

    class Impl;
    std::unique_ptr<Impl> _p;

    std::set<std::string> _pvMeters;
    std::string _gridMeter;

    std::chrono::milliseconds _primaryInterval = 5000ms;
};
