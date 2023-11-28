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

#include <common/Rpp.h>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

using namespace std::chrono_literals;

#define cfg Config::instance()

namespace toml {
class Table;
}

class ThingsRepository;

class Config {
public:
    enum class Key {
        discovery_interval,
        thing_interval,
        time_constant,
        grid,
        pv,

        name,
        pinned,
        offset,

        on,
        off,
        debounce
    };

    static Config* instance();

    template<class T>
    T valueOr(const std::string& table, Key key, T fallback = {}) const;
    void setValue(const std::string& table, Property key, const Value& value);

    inline const std::set<std::string>& pvMeters() const {
        return _pvMeters;
    }
    inline const std::string& gridMeter() const {
        return _gridMeter;
    }
    inline int discoveryInterval() const {
        return _discoveryInterval;
    }
    void setThingInterval(int seconds);
    int thingInterval() const;

    void setTimeConstant(const Value& tau);
    const behavior_subject<int>& timeConstant() const;

    double evseAlpha() const;
    double evseBeta() const;
    double evsePhi() const;

    bool _testing = false;

private:
    Config();

    void parse();

private:
    static inline Config* _instance = nullptr;

    std::string _configFile = "/var/lib/iotic/iotic.conf";

    struct Impl;
    std::unique_ptr<Impl> _p;

    std::set<std::string> _pvMeters;
    std::string _gridMeter;

    int _discoveryInterval = 60;
};
