#include "SunSpecMpptInverterExtensionModelFactory.h"

#include <cmath>

#include <sunspec/SunSpecModel.h>

namespace sunspec {

void MpptInverterExtensionModelFactory::updateFromBuffer(std::optional<Model>& model,
                                                         const std::vector<uint16_t>& buffer,
                                                         uint32_t timestamp) {
    if (buffer.size() < 8) {
        return;
    }/* else if (buffer.size() != (10 + buffer.at(8) * 20)) {
        return {};
    }*/

    // Compute the number of MPPs from the buffer size (instead taking it from the payload).
    const auto count = (buffer.size()-8)/20;

    // If no model instance present, create one
    if (!model) {
        model.emplace(Model());
        model->m_modelId = 160;
    }

    const double sfCurrent = pow(10.0, *(int16_t*)&buffer.at(0));
    const double sfVoltage = pow(10.0, *(int16_t*)&buffer.at(1));
    const double sfPower = pow(10.0, *(int16_t*)&buffer.at(2));

    std::vector<sunspec::Block<double>> dcs(count);
    for (uint16_t i = 0; i < count; ++i) {
        uint32_t dcCurrent = buffer.at(8 + (i * 20) + 9);
        if (dcCurrent == 65535) {
            dcCurrent = 0;
        }
        uint32_t dcVoltage = buffer.at(8 + (i * 20) + 10);
        if (dcVoltage == 65535) {
            dcVoltage = 0;
        }
        uint32_t dcPower = buffer.at(8 + (i * 20) + 11);
        if (dcPower == 65535) {
            dcPower = 0;
        }

        dcs[i][sunspec::current] = (double)(dcCurrent * sfCurrent);
        dcs[i][sunspec::voltage] = (double)(dcVoltage * sfVoltage);
        dcs[i][sunspec::power] = (double)(dcPower * sfPower);
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::dc] = dcs;
}

} // namespace sunspec
