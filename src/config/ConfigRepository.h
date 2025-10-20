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

#include <uvw_iot/ThingProperty.h>
#include <uvw_iot/ThingRepository.h>

namespace toml {
class Table;
}

using namespace std::chrono_literals;
using uvw_iot::ThingPtr;
using uvw_iot::ThingPropertyKey;
using uvw_iot::ThingPropertyValue;
using uvw_iot::ThingRepository;

class ConfigRepository {
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
        debounce,
        priority,
    };

    ConfigRepository(const ThingRepository& repo);
    ~ConfigRepository();

    template<class T>
    T valueOr(const std::string& table, Key key, T fallback = {}) const;

    template<class T>
    T valueOr(Key key, T fallback = {}) const;

    inline const std::set<std::string>& pvMeters() const {
        return _pvMeters;
    }
    inline const std::string& gridMeter() const {
        return _gridMeter;
    }
    inline int discoveryInterval() const {
        return _discoveryInterval;
    }

    void setThingInterval(const ThingPropertyValue& value) const;
    dynamic_observable<int> thingIntervalObservable() const;
    int thingInterval() const;

private:
    void parseConfigFile();

    void setPropertiesTo(const ThingPtr& thing);

    std::optional<ThingPropertyValue> value(const std::string& id, ThingPropertyKey key) const;

    void persistProperty(const std::string& table, ThingPropertyKey key, const ThingPropertyValue& value) const;

private:
    const ThingRepository& _repo;
    const std::string _configFile = "/var/lib/iotic/iotic.conf";

    class Impl;
    mutable std::unique_ptr<Impl> _p;

    std::set<std::string> _pvMeters;
    std::string _gridMeter;

    int _discoveryInterval = 60;
};
