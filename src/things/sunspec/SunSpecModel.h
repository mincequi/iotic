#pragma once

#include <map>

#include <magic_enum.hpp>

#include "SunSpecDataPoint.h"
#include "SunSpecStatsValue.h"
#include "models/SunSpecCommonModelFactory.h"

namespace sunspec {

class Model {
public:
    // https://sunspec.org/wp-content/uploads/2015/06/SunSpec-Specification-Common-Models-A12031-1.6.pdf
    enum Id : uint16_t {
        InverterSinglePhase = 101, // (Integer+SF)
        InverterSplitPhase = 102, // (Integer+SF)
        InverterThreePhase = 103, // (Integer+SF)
        //InverterSinglePhaseModelFloat = 111, // (Floats)
        //InverterSplitPhaseModelFloat = 112, // (Floats)
        //InverterThreePhaseModelFloat = 113, // (Floats)
        //InverterControlsNameplateRatings = 120,
        //InverterControlsBasicSettings = 121,
        //InverterControlsMeasurementsAndStatus = 122,
        //InverterControlsImmediateControls = 123,
        //InverterControlsBasicStorageControl = 124,
        //InverterControlsPricingSignal = 125,
        //InverterControlsStaticVoltVarArrays = 126,
        //InverterControlsFrequencyWattControl = 127,
        //InverterControlsDynamicReactiveCurrent = 128,
        //InverterControlsLvrtArrays = 129,
        //InverterControlsHvrtArrays = 130,
        //InverterControlsWattPowerFactorArrays = 131,
        //InverterControlsVoltageWattArrays = 132,
        //InverterControlsBasicScheduling = 133,

        //Multiple MPPT Inverter Extension Model
        InverterMpptExtension = 160,

        MeterSinglePhase = 201, // (AN or AB) meter
        MeterSplitSinglePhase = 202, // (ABN) meter
        MeterWyeConnectThreePhase = 203, // (abcn) meter

        /*
        delta-connect three phase (abc) meter = 204,
        single phase (AN or AB) meter = 211,
        split single phase (ABN) meter = 212,
        wye-connect three phase (abcn) meter = 213,
        delta-connect three phase (abc) meter = 214,
        */
    };

    bool operator==(const Model& other) const;
    bool operator!=(const Model& other) const;

    uint16_t modelId() const;

    const std::map<DataPoint, LiveValue>& values() const;
    LiveValue& operator[](DataPoint i);

protected:
    uint16_t _modelId = 0;

    std::map<DataPoint, LiveValue> _values;

    friend class ModelFactory;
    friend class ::SunSpecCommonModelFactory;
    friend class ElgrisSmartMeterModelFactory;
    friend class InverterModelFactory;
    friend class MpptInverterExtensionModelFactory;
    friend class WyeConnectMeterModelFactory;
};

} // namespace sunspec

// We have to inject a custom range for this enum, since magic_enum only allows -128 to 128 per default.
template<>
struct magic_enum::customize::enum_range<sunspec::Model::Id> {
    static constexpr int min = (int)sunspec::Model::Id::InverterSinglePhase;
    static constexpr int max = (int)sunspec::Model::Id::MeterWyeConnectThreePhase;
    // (max - min) must be less than UINT16_MAX.
};
