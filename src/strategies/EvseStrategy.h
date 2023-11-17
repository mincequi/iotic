#pragma once

#include <strategies/Strategy.h>
#include <things/Thing.h>

class EvseStrategy : public Strategy {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing);
    ~EvseStrategy();

private:
    enum class Phases {
        off = 0,
        single_phase = 1,
        three_phase = 3
    };

    EvseStrategy(const ThingPtr& thing);
    void evaluate(const std::map<Property, Value>& siteProperties) override;

    Phases computePhases() const;
    Value computeCurrent() const;

    double _power = 0;
    std::array<double, 3> _voltages;
    double _gridPower = 0;
    double _offsetPower = 0;
    // Short term is used for amperage
    double _shortTermAvailablePower = 0.0;
    // Long term is used for phases
    double _longTermAvailablePower = 0.0;

    rpp::subjects::publish_subject<bool> _wantsToSwitch;
    std::optional<Phases> _phases;
};
