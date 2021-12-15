#include "SunSpecWyeConnectMeterModelFactory.h"

#include <cmath>
#include <vector>

namespace sunspec {

void WyeConnectMeterModelFactory::updateFromBuffer(std::optional<Model>& model,
                                                   const std::vector<uint16_t>& buffer,
                                                   uint32_t timestamp) {
    if (buffer.size() != 107) {
        return;
    }

    const int16_t sunssf = *(int16_t*)&buffer.at(22) * -1;
    const double totalActivePower = *(int16_t*)&buffer.at(18) * pow(10.0, sunssf);

    if (!model) {
        model.emplace(Model());
        model->m_modelId = buffer.front();
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::totalActiveAcPower] = totalActivePower;
    model.value().m_values[sunspec::totalExportedActiveEnergy] = (int32_t)round((buffer.at(38) + (buffer.at(39) << 16)) / 100.0) * 100.0;
    model.value().m_values[sunspec::totalImportedActiveEnergy] = (int32_t)round((buffer.at(46) + (buffer.at(47) << 16)) / 100.0) * 100.0;
}

} // namespace sunspec
