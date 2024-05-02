#pragma once

#include <uvw_iot/ThingRepository.h>

#include "Strategy.h"

namespace uvw_iot {
class Site;
}

using uvw_iot::Site;
using uvw_iot::ThingPtr;
using uvw_iot::ThingPropertyValue;
using uvw_iot::ThingRepository;

class Config;

class EvseStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing,
                                          const ThingRepository& repo,
                                          const Site& site,
                                          const Config& cfg);
    ~EvseStrategy();

private:
    enum class Phases {
        off = 0,
        single_phase = 1,
        three_phase = 3
    };

    EvseStrategy(const ThingPtr& thing,
                 const ThingRepository& repo,
                 const Site& site,
                 const Config& cfg);
    void evaluate(double gridPower) override;

    Phases computePhases() const;
    ThingPropertyValue computeCurrent() const;

    const ThingRepository& _repo;
    const Site& _site;
    const Config& _cfg;

    double _power = 0;
    std::array<int, 3> _voltages;
    double _offsetPower = 0;
    // Short term is used for amperage
    double _shortTermAvailablePower = 0.0;
    // Long term is used for phases
    double _longTermAvailablePower = 0.0;

    rpp::subjects::publish_subject<bool> _wantsToSwitch;
    std::optional<Phases> _phases;
};
