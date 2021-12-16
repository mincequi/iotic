#include "SunSpecInverterModelFactory.h"

#include <cmath>
#include <vector>

namespace sunspec {

void InverterModelFactory::updateFromBuffer(std::optional<Model>& model,
                                            const std::vector<uint16_t>& buffer,
                                            uint32_t timestamp) {
    if (buffer.size() != 50) {
        return;
    }

    const auto sfPower = *(int16_t*)&buffer.at(13);
    int32_t totalActiveAcPower = *(int16_t*)&buffer.at(12);
    totalActiveAcPower = totalActiveAcPower < 0 ? 0 : totalActiveAcPower;
    totalActiveAcPower *= pow(10.0, sfPower);

    const auto sfYield = *(int16_t*)&buffer.at(24);
    int32_t totalYield = (buffer.at(22) << 16) + buffer.at(23);
    totalYield *= pow(10.0, sfYield);

    if (!model) {
        model.emplace(Model());
        model->m_modelId = 103;
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::totalActiveAcPower] = (double)(totalActiveAcPower);
    model.value().m_values[sunspec::totalExportedActiveEnergy] = (int32_t)(round(totalYield/100.0)*100.0);
    model.value().m_values[sunspec::operatingState] = static_cast<InverterOperatingState>(buffer.at(36));
    model.value().m_values[sunspec::events] = InverterEvents(buffer.at(38));
}

} // namespace sunspec
