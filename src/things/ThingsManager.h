#pragma once

#include <cstdint>

namespace uvw_iot {
class ThingRepository;
} // namespace uvw_iot
class ConfigRepository;

using uvw_iot::ThingRepository;

// TODO: ThingsManager should probably not belong to things (due to cyclic dependencies).
class ThingsManager {
public:
    explicit ThingsManager(const ThingRepository& thingsRepository, const ConfigRepository& cfg);

private:
    void onTimer() const;

    const ThingRepository& _thingRepository;
    const ConfigRepository& _configRepository;
};
