#pragma once

#include <array>

#include <magic_enum.hpp>

#include <rpp/subjects/publish_subject.hpp>

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
    EvseStrategy(const ThingPtr& thing);

    enum class Phases {
        off = 0,
        single_phase = 1,
        three_phase = 3
    };

    void evaluate() override;

    //EvseData _chargingData;
    double _power = 0;
    std::array<double, 3> _voltages;
    double _gridPower = 0;
    double _offsetPower = 0;
    double _availablePower = 0.0;

    rpp::subjects::publish_subject<Phases> _phasesSubject;
    std::optional<Phases> _phasesState;

    //rpp::subjects::publish_subject<bool> _isOnSubject;
    //std::optional<bool> _isOnState;
};
