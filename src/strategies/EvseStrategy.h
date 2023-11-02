#pragma once

#include <array>

#include <magic_enum.hpp>

#include <strategies/Strategy.h>
#include <things/Thing.h>

class EvseStrategy : public Strategy {
public:
    struct EvseData {
        std::array<double, 3> voltages;
        double power = 0;
    };

    static std::unique_ptr<Strategy> from(const ThingPtr& thing);
    ~EvseStrategy();

    void setChargingData(const EvseData& data);

private:
    enum class Phases {
        off = 0,
        single_phase = 1,
        three_phase = 3
    };

    EvseStrategy(const ThingPtr& thing);
    void evaluate() override;

    Phases computePhases() const;
    Value computeCurrent() const;

    //EvseData _chargingData;
    double _power = 0;
    std::array<double, 3> _voltages;
    double _gridPower = 0;
    double _offsetPower = 0;
    // Short term is used for amperage
    double _shortTermAvailablePower = 0.0;
    double _shortTermAlpha = 2.0/3.0;
    // Long term is used for phases
    double _longTermAvailablePower = 0.0;
    // https://en.wikipedia.org/wiki/Time_constant
    // alpha = 1 - e^{-deltaT/tau }
    // deltaT = 2s, tau = 5minutes * 60seconds / 3tau(95%) / 2(correction)
    double _longTermAlpha = 0.039210560847677;

    rpp::subjects::publish_subject<bool> _wantsToSwitch;
    std::optional<Phases> _phases;
};
