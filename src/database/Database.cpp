#include "Database.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <magic_enum.hpp>
#include <rfl/json.hpp>

#include <mdbx.h++>

#include <uvw_iot/ThingRepository.h>

#include <common/Logger.h>

#include "ArchiveDayUseCase.h"
#include "DatabaseUtil.h"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace mdbx;
using namespace uvw_iot;

class DatabasePrivate {
public:
    DatabasePrivate(Database& self, const Database::Config& config) : _self(self) {
        env_managed::create_parameters cp;
        cp.geometry = env::geometry::kB;
        cp.file_mode_bits = 0664;
        env::operate_parameters op;
        op.max_maps = 32;
        _dbEnv = mdbx::env_managed(config.dbFile, cp, op);
    }

    void putDatapoint(const std::string& thingId, ThingPropertyKey property, uint32_t timestamp, const Datapoint& datapoint) {
        // skip writing if value hasn't changed
        auto& lastValue = _lastValues[thingId][property];
        if (lastValue == datapoint) {
            return;
        }
        _lastValues[thingId][property] = datapoint;

        // obtain map
        auto& map = createMap(thingId, property);
        auto txn = _dbEnv.start_write();
        txn.put(map, mdbx::slice(&timestamp, sizeof(timestamp)), mdbx::slice(datapoint.data(), datapoint.size() * sizeof(int16_t)), put_mode::upsert);
        txn.commit();
    }

    Dataset rawData(std::string_view thingId, ThingPropertyKey property, year_month_day ymd) {
        auto map = mapFor(thingId, property);
        if (!map) {
            return {};
        }

        auto txn = _dbEnv.start_read();
        auto cursor = txn.open_cursor(map);

        // Why is slice non-const? Why pass a reference to slice?
        // Find the first entry for the given day
        const uint32_t dayStart = system_clock::to_time_t(sys_days(ymd));
        const uint32_t dayEnd = dayStart + 24 * 60 * 60;
        auto slice = mdbx::slice(&dayStart, sizeof(dayStart));
        Dataset data;

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
            //LOG_S(ERROR) << "Failed to read from database: " << e.what();
        }

        return data;
    }

    std::string_view archivedData(std::string_view thingId,
                                  ThingPropertyKey property,
                                  year_month_day day,
                                  minutes resolution) {
        auto map = mapFor(thingId, property, resolution);
        if (!map) {
            return {};
        }

        auto txn = _dbEnv.start_read();
        auto cursor = txn.open_cursor(map);

        // MDBX only supports uint32_t and uint64_t as key types, so we cannot use uint16_t
        const uint32_t daySinceEpoch = DatabaseUtil::daySinceEpoch(day);

        // Try to find archived data for the day. If not found, archive the day and try again.
        auto result = cursor.to_key_exact(mdbx::slice(&daySinceEpoch, sizeof(daySinceEpoch)), false);
        if (!result) {
            return {};
        }

        return result.value.string_view();
    }

    void putArchivedData(std::string_view thingId,
                         ThingPropertyKey property,
                         year_month_day day,
                         minutes resolution,
                         std::string_view data) {
        if (data.empty()) {
            return;
        }

        // MDBX only supports uint32_t and uint64_t as key types, so we cannot use uint16_t
        const uint32_t daySinceEpoch = DatabaseUtil::daySinceEpoch(day);

        auto& map = createMap(thingId, property, resolution);
        auto txn = _dbEnv.start_write();
        txn.put(map, mdbx::slice(&daySinceEpoch, sizeof(daySinceEpoch)), mdbx::slice(data.data(), data.size()), put_mode::upsert);
        txn.commit();
    }

    mdbx::map_handle& createMap(std::string_view thingId,
                                ThingPropertyKey key,
                                minutes resolution = minutes(0)) {
        const std::string mapName_ = mapName(thingId, key, resolution);

        auto it = _mapCache.find(mapName_);
        if (it != _mapCache.end()) return it->second;

        auto txn = _dbEnv.start_write();
        auto map = txn.create_map(mapName_, mdbx::key_mode::ordinal);
        txn.commit();

        return _mapCache.emplace(mapName_, std::move(map)).first->second;
    }

    mdbx::map_handle mapFor(std::string_view thingId, ThingPropertyKey key, minutes resolution = minutes(0)) {
        const std::string name = mapName(thingId, key, resolution);

        auto it = _mapCache.find(name);
        if (it != _mapCache.end()) return it->second;

        auto txn = _dbEnv.start_read(); //  ::begin(env, nullptr, MDBX_TXN_RDONLY);
        mdbx::map_handle map;

        try {
            map = txn.open_map(name);
        }
        catch (...) {
            return {};
        }

        return _mapCache.emplace(name, std::move(map)).first->second;
    }

    std::string mapName(std::string_view thingId, ThingPropertyKey key, minutes resolution = minutes(0)) const {
        std::string mapName = std::string{thingId} + "." + magic_enum::enum_name(key).data();
        if (resolution != minutes(0)) {
            // Add resolution suffix to map name, e.g. "thingId.power.5min"
            mapName += "." + std::to_string(resolution.count()) + "min";
        }
        return mapName;
    }

    void eraseRawData(std::string_view thingId, ThingPropertyKey key, year_month_day day) {
        const std::string name = mapName(thingId, key);

        auto map = mapFor(thingId, key);
        if (!map) {
            return;
        }

        auto txn = _dbEnv.start_write();
        auto cursor = txn.open_cursor(map);
        const uint32_t dayStart = system_clock::to_time_t(sys_days(day));
        const uint32_t dayEnd = dayStart + 24 * 60 * 60;
        auto slice = mdbx::slice(&dayStart, sizeof(dayStart));

        cursor.scan_from([&](const mdbx::pair& kv) -> bool {
            uint32_t timestamp = *reinterpret_cast<const uint32_t*>(kv.key.data());
            if (timestamp >= dayEnd) {
                return true; // stop scan
            }
            cursor.erase();
            return false; // continue scan
        }, slice);
        txn.commit();
    }

    Database& _self;
    mdbx::env_managed _dbEnv;
    std::unordered_map<std::string, mdbx::map_handle> _mapCache;
    std::unordered_map<std::string, std::unordered_map<ThingPropertyKey, Datapoint>> _lastValues;
};

Database::Database(const Database::Config& config,
                   const ThingRepository& thingRepository) :
    d(new DatabasePrivate(*this, config)) {
    thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
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
}

Database::~Database() = default;

std::string Database::info() const {
    return rfl::json::write(d->_dbEnv.get_info().mi_geo);
}

std::string Database::stat() const {
    return rfl::json::write(d->_dbEnv.get_stat());
}

std::map<std::string, uvw_iot::ThingType> Database::things() const {
    std::map<std::string, uvw_iot::ThingType> result;

    return result;
}

void Database::addThing(const std::string_view thingId, uvw_iot::ThingType thingType) {

}

std::map<std::string, int> Database::maps(const std::string& map_) const {
    std::map<std::string, int> result;

    auto txn = d->_dbEnv.start_read();
    mdbx::map_handle map;
    if (map_.empty()) {
        map = txn.open_map(nullptr);
    } else {
        map = txn.open_map(map_);
    }
    auto cursor = txn.open_cursor(map);
    cursor.fullscan([&](const mdbx::pair& p) -> bool {
        const std::string s{p.key.string_view()};
        result[s] = 0;
        return /* continue scan */ false;
    });

    return result;
}

size_t Database::mapSize(std::string_view thingId, uvw_iot::ThingPropertyKey property) const {
    auto map = d->mapFor(thingId, property);
    if (!map) {
        return 0;
    }

    auto txn = d->_dbEnv.start_read();
    return txn.get_map_stat(map).ms_entries;
}

Dataset Database::rawData(std::string_view thingId, ThingPropertyKey property, year_month_day ymd) const {
    return d->rawData(thingId, property, ymd);
}

void Database::putDatapoint(const std::string& thingId, ThingPropertyKey property, uint32_t timestamp, const Datapoint& datapoint) {
    d->putDatapoint(thingId, property, timestamp, datapoint);
}

void Database::eraseRawData(std::string_view thingId, ThingPropertyKey property, year_month_day day) {
    d->eraseRawData(thingId, property, day);
}

std::string_view Database::archivedData(std::string_view thingId,
                                        ThingPropertyKey property,
                                        year_month_day day,
                                        minutes resolution) {
    auto archivedData = d->archivedData(thingId, property, day, resolution);
    if (archivedData.empty()) {
        // If no archived data for the day, archive the day and try again
        ArchiveDayUseCase(*this)(thingId, property, day, resolution);
        archivedData = d->archivedData(thingId, property, day, resolution);
    }

    return archivedData;
}

void Database::putArchivedData(std::string_view thingId,
                               ThingPropertyKey property,
                               year_month_day day,
                               minutes resolution,
                               std::string_view data) {
    d->putArchivedData(thingId, property, day, resolution, data);
}
