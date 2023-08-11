#include "SunSpecInverterModelFactory.h"

#include <cmath>
#include <vector>

#include <things/sunspec/SunSpecModel.h>

namespace sunspec {

bool InverterModelFactory::updateFromBuffer(Model& model,
                                            const std::vector<uint16_t>& buffer,
                                            uint32_t timestamp) {
    if (buffer.size() != 50) {
        return false;
    }

    const auto sfPower = *(int16_t*)&buffer.at(13);
    int32_t totalActiveAcPower = *(int16_t*)&buffer.at(12);
    totalActiveAcPower = totalActiveAcPower < 0 ? 0 : totalActiveAcPower;
    totalActiveAcPower *= pow(10.0, sfPower);

    const auto sfYield = *(int16_t*)&buffer.at(24);
    int32_t totalYield = (buffer.at(22) << 16) + buffer.at(23);
    totalYield *= pow(10.0, sfYield);

    model.m_values[sunspec::timestamp] = timestamp;
    model.m_values[sunspec::totalActiveAcPower] = totalActiveAcPower;
    model.m_values[sunspec::totalExportedActiveEnergy] = (int32_t)(round(totalYield/100.0)*100.0);
    model.m_values[sunspec::operatingState] = static_cast<InverterOperatingState>(buffer.at(36));
    model.m_values[sunspec::events] = InverterEvents(buffer.at(38));

    return true;
}

} // namespace sunspec
