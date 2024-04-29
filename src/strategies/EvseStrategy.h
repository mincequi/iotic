#pragma once

#include <uvw_iot/common/ThingRepository.h>

#include "Strategy.h"

using uvw_iot::common::ThingPtr;
using uvw_iot::common::ThingPropertyValue;
using uvw_iot::common::ThingRepository;

class Config;
class Site;

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
