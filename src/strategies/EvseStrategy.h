#pragma once

#include <uvw_iot/ThingRepository.h>

#include "Strategy.h"

namespace uvw_iot::util {
class Site;
}

using uvw_iot::ThingPtr;
using uvw_iot::ThingPropertyValue;
using uvw_iot::ThingRepository;
using uvw_iot::util::Site;

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

    void evaluate(const Site::Properties& siteProperties) override;

    int computePhases(double availablePower);
    ThingPropertyValue computeCurrent(double availablePower) const;

    const ThingRepository& _repo;
    const Site& _site;
    const Config& _cfg;

    int _power = 0;
    std::array<int, 3> _voltages;
    int _offsetPower = 0;
    // Short term is used for amperage
    int _shortTermAvailablePower = 0;
    // Long term is used for phases
    int _longTermAvailablePower = 0;
    int _prevTs = 0;

    rpp::subjects::publish_subject<bool> _doPhaseSwitch;
    int _phases = 0;
    int _nextPhases = 0;
    std::chrono::time_point<std::chrono::system_clock> _nextSwitch;
};
