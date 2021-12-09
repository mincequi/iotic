#include "SunSpecWyeConnectMeterModel.h"

#include <cmath>

void SunSpecWyeConnectMeterModel::updateFromBuffer(std::optional<SunSpecWyeConnectMeterModel>& model,
                                                   const std::vector<uint16_t>& buffer,
                                                   uint32_t timestamp) {
    if (buffer.size() != 107) {
        return;
    }

    const int16_t sunssf = *(int16_t*)&buffer.at(22) * -1;
    const int32_t totalActivePower = *(int16_t*)&buffer.at(18) * pow(10.0, sunssf);

    if (!model) {
        model.emplace(SunSpecWyeConnectMeterModel());
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::totalActiveAcPower] = SunSpecMeasuredValue<int32_t>(totalActivePower);
    model.value().m_values[sunspec::totalExportedActiveEnergy] = (uint32_t)(buffer.at(38) + (buffer.at(39) << 16));
    model.value().m_values[sunspec::totalImportedActiveEnergy] = (uint32_t)(buffer.at(46) + (buffer.at(47) << 16));
}

const std::map<sunspec::DataPoint, sunspec::DataValue>& SunSpecWyeConnectMeterModel::values() const {
    return m_values;
}

SunSpecWyeConnectMeterModel::SunSpecWyeConnectMeterModel() {
}
