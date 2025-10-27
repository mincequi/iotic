#pragma once

#include <uvw_iot/ThingRepository.h>

#include "Strategy.h"

using uvw_iot::ThingPtr;
using uvw_iot::ThingPropertyValue;
using uvw_iot::ThingRepository;

class ConfigRepository;

class EvseStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const ConfigRepository& cfg);
    ~EvseStrategy();

private:
    enum class Phases {
        off = 0,
        single_phase = 1,
        three_phase = 3
    };

    EvseStrategy(const ThingPtr& thing,
                 const ThingRepository& repo,
                 const ConfigRepository& cfg);

    bool wantsToTurnOff(const Site::Properties& siteProperties) override;
    bool wantsToTurnOn(const Site::Properties& siteProperties) override;
    json toJson() const override;

    int computePhases(double availablePower) const;
    ThingPropertyValue computeCurrent(double availablePower);

    void actuate(const Site::Properties& siteProperties);
    int powerError() const;

    const ThingRepository& _thingRepository;
    const ConfigRepository& _configRepository;

    int _measuredPower = 0;
    std::array<int, 3> _voltages;
    int _offsetPower = 0;
    std::chrono::time_point<std::chrono::system_clock> _lastActuationTs;

    int _phases = 0;
    int _current = 0;

    mutable int _nextPhases = 0;
    std::chrono::time_point<std::chrono::system_clock> _nextSwitch;
};
