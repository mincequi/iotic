#include "Database.h"

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#include <magic_enum.hpp>
#include <rfl/json.hpp>

#include <mdbx.h++>

#include <uvw_net/sunspec/SunSpecTypes.h>
#include <uvw_iot/ThingRepository.h>

using namespace mdbx;
using namespace uvw_iot;
using namespace uvw_net::sunspec;

class DatabasePrivate {
public:
    DatabasePrivate() {
        // For designated Initializers we need c++20, but we stick with c++17 due to clipp.
        //env_managed::create_parameters cp {
        //    .geometry = env::geometry::GB,
        //    .file_mode_bits = 0664
        //};
        env_managed::create_parameters cp;
        cp.geometry = env::geometry::kB;
        cp.file_mode_bits = 0664;
        env::operate_parameters op(16);
        op.max_maps = 16;
        _dbEnv = mdbx::env_managed(_dbFile, cp, op);
    }

    void addDataPoint(const std::string& thingId, ThingPropertyKey key, uint32_t timestamp, Database::DataPoint dataPoint) {
        // Check if value has changed
        auto& lastValue = _lastValues[thingId][key];
        if (lastValue.index() == dataPoint.index() && lastValue == dataPoint) {
            return; // skip writing if value hasn't changed
        }
        _lastValues[thingId][key] = dataPoint;

        auto& map = mapFor(thingId, key);
        auto txn = _dbEnv.start_write();
        // We have to distinguish between int16_t and vector<int16_t>
        if (std::holds_alternative<int16_t>(dataPoint)) {
            int16_t value = std::get<int16_t>(dataPoint);
            txn.insert(map, mdbx::slice(&timestamp, sizeof(timestamp)), mdbx::slice(&value, sizeof(value)));
        } else {
            const auto& vec = std::get<std::vector<int16_t>>(dataPoint);
            txn.insert(map, mdbx::slice(&timestamp, sizeof(timestamp)), mdbx::slice(vec.data(), vec.size() * sizeof(int16_t)));
        }
        txn.commit();
    }

    mdbx::map_handle& mapFor(std::string_view thingId, ThingPropertyKey key) {
        const std::string mapName = std::string{thingId} + "." + magic_enum::enum_name(key).data();

        auto it = _mapCache.find(mapName);
        if (it != _mapCache.end()) return it->second;

        auto txn = _dbEnv.start_write();
        auto map = txn.create_map(mapName, mdbx::key_mode::ordinal);
        txn.commit();

        return _mapCache.emplace(mapName, std::move(map)).first->second;
    }

    bool hasMap(std::string_view thingId, ThingPropertyKey key) const {
        const std::string mapName = std::string{thingId} + "." + magic_enum::enum_name(key).data();

        auto it = _mapCache.find(mapName);
        if (it != _mapCache.end()) return true;

        auto txn = _dbEnv.start_read(); //  ::begin(env, nullptr, MDBX_TXN_RDONLY);
        try {
            auto map = txn.open_map(mapName);
            //_mapCache.emplace(mapName, std::move(map));
            return true;
        }
        catch (...) {
            return false;
        }
    }

    const std::string _dbFile = "/var/lib/iotic/iotic.mdbx";
    mdbx::env_managed _dbEnv;
    std::unordered_map<std::string, mdbx::map_handle> _mapCache;
    std::unordered_map<std::string, std::map<ThingPropertyKey, Database::DataPoint>> _lastValues;
};

Database::Database(const ThingRepository& thingRepository) : d(new DatabasePrivate) {
    thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
        if (thing->type() != ThingType::SolarInverter && thing->type() != ThingType::SmartMeter) {
            return;
        }

        thing->propertiesObservable().subscribe([this, thing](const ThingPropertyMap& props) {
            if (thing->type() == ThingType::SolarInverter) {
                auto status =  static_cast<InverterOperatingStatus>(props.get<ThingPropertyKey::status>().value_or(0));
                if (status != InverterOperatingStatus::starting &&
                    status != InverterOperatingStatus::mpp &&
                    status != InverterOperatingStatus::throttled &&
                    status != InverterOperatingStatus::shuttingDown) {
                    return;
                }
            }

            props.template on<uvw_iot::ThingPropertyKey::power>([this, thing](const auto& value) {
                d->addDataPoint(thing->id(), ThingPropertyKey::power, thing->lastSeen(), (int16_t)((value + 5) / 10));
            }).template on<uvw_iot::ThingPropertyKey::dcPower>([this, thing](const std::vector<int>& value) {
                // convert value to vector of int16_t
                std::vector<int16_t> convertedValue(value.size());
                std::transform(value.begin(), value.end(), convertedValue.begin(), [](int v) { return (v + 5) / 10; });
                d->addDataPoint(thing->id(), ThingPropertyKey::dcPower, thing->lastSeen(), convertedValue);
            });
        });
    });

    using namespace std::chrono;

    auto today = floor<days>(system_clock::now());
    year_month_day ymd = today;
}

Database::~Database() = default;

std::string Database::info() const {
    return rfl::json::write(d->_dbEnv.get_info().mi_geo);
}

std::string Database::stat() const {
    return rfl::json::write(d->_dbEnv.get_stat());
}

bool Database::hasMap(std::string_view thingId, ThingPropertyKey key) const {
    return d->hasMap(thingId, key);
}

std::map<uint32_t, Database::DataPoint> Database::dailyData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& ymd) const {
    if (!d->hasMap(thingId, property)) return {};

    auto& map = d->mapFor(thingId, property);
    auto txn = d->_dbEnv.start_read();
    auto cursor = txn.open_cursor(map);

    // Find the first entry for the given day
    uint32_t dayStart = std::chrono::system_clock::to_time_t(std::chrono::sys_days(ymd));
    uint32_t dayEnd = dayStart + 24 * 60 * 60;

    std::map<uint32_t, DataPoint> data;
    // Why is slice non-const? Why pass a reference to slice?
    auto slice = mdbx::slice(&dayStart, sizeof(dayStart));
    try {
        cursor.scan_from([&](const mdbx::pair& kv) -> bool {
            uint32_t timestamp = *reinterpret_cast<const uint32_t*>(kv.key.data());
            if (timestamp >= dayEnd) {
                return true; // stop scan
            }
            DataPoint value;
            if (kv.value.size() == sizeof(int16_t)) {
                value = *reinterpret_cast<const int16_t*>(kv.value.data());
            } else {
                std::vector<int16_t> vec(kv.value.size() / sizeof(int16_t));
                std::memcpy(vec.data(), kv.value.data(), kv.value.size());
                value = std::move(vec);
            }
            data[timestamp] = value;

            return false; // continue scan
        }, slice);
    } catch (...) {
         // No data for the day, return empty map
         return {};
    }

    return data;
}
