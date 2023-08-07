#pragma once

#include <map>

#include "SunSpecDataPoint.h"
#include "SunSpecStatsValue.h"

namespace sunspec {

class Model {
public:
    enum Id : uint16_t {
        SinglePhaseInverterModel = 101, // (Integer+SF)
        SplitPhaseInverterModel = 102, // (Integer+SF)
        ThreePhaseInverterModel = 103, // (Integer+SF)
        //SinglePhaseInverterModelFloat = 111, // (Floats)
        //SplitPhaseInverterModelFloat = 112, // (Floats)
        //ThreePhaseInverterModelFloat = 113, // (Floats)
        InverterControlsNameplateRatings = 120,
        InverterControlsBasicSettings = 121,
        InverterControlsMeasurementsAndStatus = 122,
        InverterControlsImmediateControls = 123,
        InverterControlsBasicStorageControl = 124,
        InverterControlsPricingSignal = 125,
        InverterControlsStaticVoltVarArrays = 126,
        InverterControlsFrequencyWattControl = 127,
        InverterControlsDynamicReactiveCurrent = 128,
        InverterControlsLvrtArrays = 129,
        InverterControlsHvrtArrays = 130,
        InverterControlsWattPowerFactorArrays = 131,
        InverterControlsVoltageWattArrays = 132,
        InverterControlsBasicScheduling = 133,

        WyeConnectMeterModel = 203
    };

    bool operator==(const Model& other) const;
    bool operator!=(const Model& other) const;

    uint16_t modelId() const;

    const std::map<DataPoint, LiveValue>& values() const;
    LiveValue& operator[](DataPoint i);

protected:
    uint16_t m_modelId = 0;

    std::map<DataPoint, LiveValue> m_values;

    friend class ModelFactory;
    friend class SunSpecCommonModelFactory;
    friend class ElgrisSmartMeterModelFactory;
    friend class InverterModelFactory;
    friend class MpptInverterExtensionModelFactory;
    friend class WyeConnectMeterModelFactory;
};

} // namespace sunspec

