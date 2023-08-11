#include "SunSpecWyeConnectMeterModelFactory.h"

#include <cmath>
#include <vector>

#include <things/sunspec/SunSpecModel.h>

namespace sunspec {

bool WyeConnectMeterModelFactory::updateFromBuffer(Model& model,
                                                   const std::vector<uint16_t>& buffer,
                                                   uint32_t timestamp) {
    if (buffer.size() != 105) {
        return false;
    }

    const int16_t sunssf = *(int16_t*)&buffer.at(20);
    const double totalActivePower = *(int16_t*)&buffer.at(16) * pow(10.0, sunssf);

    model.m_values[sunspec::timestamp] = timestamp;
    model.m_values[sunspec::totalActiveAcPower] = totalActivePower;
    model.m_values[sunspec::totalExportedActiveEnergy] = (int32_t)round(((buffer.at(36) << 16) + buffer.at(37)) / 100.0) * 100.0;
    model.m_values[sunspec::totalImportedActiveEnergy] = (int32_t)round(((buffer.at(44) << 16) + buffer.at(45)) / 100.0) * 100.0;

    return true;
}

} // namespace sunspec
