#pragma once

#include <array>

#include <magic_enum.hpp>

#include <rpp/subjects/publish_subject.hpp>

#include <strategies/Strategy.h>

class EvseStrategy : public Strategy {
public:
    struct EvseData {
        std::array<double, 3> voltages;
        int power = 0;
    };

    EvseStrategy(const std::string& thingId);
    ~EvseStrategy();

    void setChargingData(const EvseData& data);
    void setGridPower(int watts);
    void setOffsetPower(int watts);

private:
    enum class Phases {
        Off = 0,
        SinglePhase = 1,
        ThreePhase = 3
    };

    void evaluate() override;

    EvseData _chargingData;
    int _gridPower = 0;
    int _offsetPower = 0;

    rpp::subjects::publish_subject<Phases> _phasesSubject;
    std::optional<Phases> _phasesState;
};
