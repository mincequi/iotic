#include "Database.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <magic_enum.hpp>
#include <rfl/json.hpp>

#include <mdbx.h++>

#include <uvw_iot/ThingRepository.h>

#include "ArchiveDayUseCase.h"
#include "DatabaseUtil.h"

using namespace std::chrono_literals;
using namespace mdbx;
using namespace uvw_iot;

class DatabasePrivate {
public:
    DatabasePrivate(Database& self) : _self(self) {
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

    void putDatapoint(const std::string& thingId, ThingPropertyKey property, uint32_t timestamp, const Datapoint& datapoint) {
        // skip writing if value hasn't changed
        auto& lastValue = _lastValues[thingId][property];
        if (lastValue == datapoint) {
            return;
        }
        _lastValues[thingId][property] = datapoint;

        // obtain map
        auto& map = mapFor(thingId, property);
        auto txn = _dbEnv.start_write();
        txn.put(map, mdbx::slice(&timestamp, sizeof(timestamp)), mdbx::slice(datapoint.data(), datapoint.size() * sizeof(int16_t)), put_mode::upsert);
        txn.commit();
    }

    Dataset rawData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& ymd) {
        if (!hasMap(thingId, property)) return {};

        // Find the first entry for the given day
        const uint32_t dayStart = std::chrono::system_clock::to_time_t(std::chrono::sys_days(ymd));
        const uint32_t dayEnd = dayStart + 24 * 60 * 60;

        auto& map = mapFor(thingId, property);
        auto txn = _dbEnv.start_read();
        auto cursor = txn.open_cursor(map);

        Dataset data;
        // Why is slice non-const? Why pass a reference to slice?
        auto slice = mdbx::slice(&dayStart, sizeof(dayStart));
        try {
            cursor.scan_from([&](const mdbx::pair& kv) -> bool {
                uint32_t timestamp = *reinterpret_cast<const uint32_t*>(kv.key.data());
                if (timestamp >= dayEnd) {
                    return true; // stop scan
                }
                Datapoint value;
                std::vector<int16_t> vec(kv.value.size() / sizeof(int16_t));
                std::memcpy(vec.data(), kv.value.data(), kv.value.size());
                value = std::move(vec);
                data[timestamp] = value;

                return false; // continue scan
            }, slice);
        } catch (...) {
            // No data for the day, return empty map
            return {};
        }

        return data;
    }

    std::string_view archivedData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& day) {
        // MDBX only supports uint32_t and uint64_t as key types, so we cannot use uint16_t
        const uint32_t daySinceEpoch = DatabaseUtil::daySinceEpoch(day);

        auto& map = mapFor(thingId, property, 1min);
        auto txn = _dbEnv.start_read();
        auto cursor = txn.open_cursor(map);

        // Try to find archived data for the day. If not found, archive the day and try again.
        auto result = cursor.to_key_exact(mdbx::slice(&daySinceEpoch, sizeof(daySinceEpoch)), false);
        if (!result) {
            return {};
        }

        return result.value.string_view();
    }

    void putArchivedData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& day, std::string_view data) {
        if (data.empty()) {
            return;
        }

        // MDBX only supports uint32_t and uint64_t as key types, so we cannot use uint16_t
        const uint32_t daySinceEpoch = DatabaseUtil::daySinceEpoch(day);

        auto& map = mapFor(thingId, property, 1min);
        auto txn = _dbEnv.start_write();
        txn.put(map, mdbx::slice(&daySinceEpoch, sizeof(daySinceEpoch)), mdbx::slice(data.data(), data.size()), put_mode::upsert);
        txn.commit();
    }

    mdbx::map_handle& mapFor(std::string_view thingId, ThingPropertyKey key, std::chrono::minutes resolution = std::chrono::minutes(0)) {
        const std::string mapName_ = mapName(thingId, key, resolution);

        auto it = _mapCache.find(mapName_);
        if (it != _mapCache.end()) return it->second;

        auto txn = _dbEnv.start_write();
        auto map = txn.create_map(mapName_, mdbx::key_mode::ordinal);
        txn.commit();

        return _mapCache.emplace(mapName_, std::move(map)).first->second;
    }

    bool hasMap(std::string_view thingId, ThingPropertyKey key, std::chrono::minutes resolution = std::chrono::minutes(0)) const {
        const std::string map = mapName(thingId, key, resolution);

        auto it = _mapCache.find(map);
        if (it != _mapCache.end()) return true;

        auto txn = _dbEnv.start_read(); //  ::begin(env, nullptr, MDBX_TXN_RDONLY);
        try {
            txn.open_map(map);
            return true;
        }
        catch (...) {
            return false;
        }
    }

    std::string mapName(std::string_view thingId, ThingPropertyKey key, std::chrono::minutes resolution = std::chrono::minutes(0)) const {
        std::string mapName = std::string{thingId} + "." + magic_enum::enum_name(key).data();
        if (resolution != std::chrono::minutes(0)) {
            // Add resolution suffix to map name, e.g. "thingId.power.5min"
            mapName += "." + std::to_string(resolution.count()) + "min";
        }
        return mapName;
    }

    Database& _self;
    const std::string _dbFile = "/var/lib/iotic/iotic.mdbx";
    mdbx::env_managed _dbEnv;
    std::unordered_map<std::string, mdbx::map_handle> _mapCache;
    std::unordered_map<std::string, std::unordered_map<ThingPropertyKey, Datapoint>> _lastValues;
};

Database::Database(const ThingRepository& thingRepository) : d(new DatabasePrivate(*this)) {
    thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
        //if (thing->type() != ThingType::SolarInverter &&
        //    thing->type() != ThingType::SmartMeter &&
        //    thing->type() != ThingType::Relay) {
        //    return;
        //}

        thing->propertiesObservable().subscribe([this, thing](const ThingPropertyMap& props) {
            props.template on<ThingPropertyKey::power>([this, thing](const auto& value) {
                d->putDatapoint(thing->id(), ThingPropertyKey::power, thing->lastSeen(), {(int16_t)((value + 5) / 10)});
            }).template on<ThingPropertyKey::dcPower>([this, thing](const std::vector<int>& value) {
                // convert value to vector of int16_t
                std::vector<int16_t> convertedValue(value.size());
                std::transform(value.begin(), value.end(), convertedValue.begin(), [](int v) { return (v + 5) / 10; });
                d->putDatapoint(thing->id(), ThingPropertyKey::dcPower, thing->lastSeen(), convertedValue);
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

Dataset Database::rawData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& ymd) const {
    return d->rawData(thingId, property, ymd);
}

void Database::putDatapoint(const std::string& thingId, ThingPropertyKey property, uint32_t timestamp, const Datapoint& datapoint) {
    d->putDatapoint(thingId, property, timestamp, datapoint);
}

std::string_view Database::archivedData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& day) {
    auto archivedData = d->archivedData(thingId, property, day);
    if (archivedData.empty()) {
        // If no archived data for the day, archive the day and try again
        ArchiveDayUseCase(*this)(thingId, property, day);
        archivedData = d->archivedData(thingId, property, day);
    }

    return archivedData;
}

void Database::putArchivedData(std::string_view thingId, ThingPropertyKey property, const std::chrono::year_month_day& day, std::string_view data) {
    d->putArchivedData(thingId, property, day, data);
}
