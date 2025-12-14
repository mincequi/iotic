#pragma once

#include <uvw_iot/ThingStatus.h>

#include "Strategy.h"

using uvw_iot::ThingPtr;
using uvw_iot::ThingPropertyValue;
using uvw_iot::ThingStatus;

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

    json toJson() const override;

    bool wantsToStepDown(const Site::Properties& siteProperties) const override;
    bool wantsToStepUp(const Site::Properties& siteProperties) const override;
    void adjust(Step step, const Site::Properties& siteProperties) override;

    int computePhases(double availablePower, bool applyHysteresis = false) const;
    int computeCurrent(double availablePower);

    int powerError() const;

    void onStatus(ThingStatus status);

    const ThingRepository& _thingRepository;
    const ConfigRepository& _configRepository;

    double _measuredPower = 0.0;
    std::chrono::milliseconds _lastMeasurementTs;
    std::array<int, 3> _voltages;
    int _offsetPower = 0;
    ThingStatus _status = ThingStatus::unknown;

    int _phases = 0;
    int _current = 0;

    mutable int _nextPhases = 0;

    const int _onePhaseHysteresis = 0;
    const int _threePhaseHysteresis = 0;
};
