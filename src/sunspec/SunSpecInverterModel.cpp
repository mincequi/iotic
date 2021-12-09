#include "SunSpecInverterModel.h"

#include <cmath>

void SunSpecInverterModel::updateFromBuffer(std::optional<SunSpecInverterModel>& model,
                                            const std::vector<uint16_t>& buffer,
                                            uint32_t timestamp) {
    if (buffer.size() != 52) {
        return;
    }

    const auto sunssf = *(int16_t*)&buffer.at(15);
    int32_t totalActiveAcPower = *(int16_t*)&buffer.at(14);
    totalActiveAcPower = totalActiveAcPower < 0 ? 0 : totalActiveAcPower;
    totalActiveAcPower *= pow(10.0, sunssf);

    const auto sfYield = *(int16_t*)&buffer.at(26);
    uint32_t totalYield = (buffer.at(24) << 16) + buffer.at(25);
    totalYield *= pow(10.0, sfYield);

    if (!model) {
        model.emplace(SunSpecInverterModel());
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::totalActiveAcPower] = SunSpecMeasuredValue<int32_t>(totalActiveAcPower);
    model.value().m_values[sunspec::totalExportedActiveEnergy] = SunSpecMeasuredValue<uint32_t>(totalYield);
}

const std::map<sunspec::DataPoint, sunspec::DataValue>& SunSpecInverterModel::values() const {
    return m_values;
}

SunSpecInverterModel::SunSpecInverterModel() {
}
