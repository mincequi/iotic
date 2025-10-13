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

    void evaluate(const Site::Properties& siteProperties) const override;
    bool wantsToTurnOff(const Site::Properties& siteProperties) override;
    bool wantsToTurnOn(const Site::Properties& siteProperties) override;

    int computePhases(double availablePower) const;
    ThingPropertyValue computeCurrent(double availablePower) const;

    const ThingRepository& _repo;
    const ConfigRepository& _cfg;

    int _power = 0;
    std::array<int, 3> _voltages;
    int _offsetPower = 0;
    // Short term is used for amperage
    mutable int _shortTermAvailablePower = 0;
    // Long term is used for phases
    mutable int _longTermAvailablePower = 0;
    mutable int _prevTs = 0;

    rpp::subjects::publish_subject<bool> _doPhaseSwitch;
    int _phases = 0;
    mutable int _nextPhases = 0;
    std::chrono::time_point<std::chrono::system_clock> _nextSwitch;
};
