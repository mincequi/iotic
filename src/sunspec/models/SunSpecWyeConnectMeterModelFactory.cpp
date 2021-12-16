#include "SunSpecWyeConnectMeterModelFactory.h"

#include <cmath>
#include <vector>

namespace sunspec {

void WyeConnectMeterModelFactory::updateFromBuffer(std::optional<Model>& model,
                                                   const std::vector<uint16_t>& buffer,
                                                   uint32_t timestamp) {
    if (buffer.size() != 105) {
        return;
    }

    const int16_t sunssf = *(int16_t*)&buffer.at(20) * -1;
    const double totalActivePower = *(int16_t*)&buffer.at(16) * pow(10.0, sunssf);

    if (!model) {
        model.emplace(Model());
        model->m_modelId = 203;
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::totalActiveAcPower] = totalActivePower;
    model.value().m_values[sunspec::totalExportedActiveEnergy] = (int32_t)round((buffer.at(36) + (buffer.at(37) << 16)) / 100.0) * 100.0;
    model.value().m_values[sunspec::totalImportedActiveEnergy] = (int32_t)round((buffer.at(44) + (buffer.at(45) << 16)) / 100.0) * 100.0;
}

} // namespace sunspec
