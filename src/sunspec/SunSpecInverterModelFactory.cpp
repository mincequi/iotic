#include "SunSpecInverterModelFactory.h"

#include <cmath>
#include <vector>

namespace sunspec {

void InverterModelFactory::updateFromBuffer(std::optional<Model>& model,
                                               const std::vector<uint16_t>& buffer,
                                               uint32_t timestamp) {
    if (buffer.size() != 52) {
        return;
    }

    const auto sfPower = *(int16_t*)&buffer.at(15);
    int32_t totalActiveAcPower = *(int16_t*)&buffer.at(14);
    totalActiveAcPower = totalActiveAcPower < 0 ? 0 : totalActiveAcPower;
    totalActiveAcPower *= pow(10.0, sfPower);

    const auto sfYield = *(int16_t*)&buffer.at(26);
    int32_t totalYield = (buffer.at(24) << 16) + buffer.at(25);
    totalYield *= pow(10.0, sfYield);

    if (!model) {
        model.emplace(Model());
        model->m_modelId = buffer.front();
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::totalActiveAcPower] = (double)(totalActiveAcPower);
    model.value().m_values[sunspec::totalExportedActiveEnergy] = totalYield;
}

} // namespace sunspec
