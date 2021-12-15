#include "SunSpecMpptInverterExtensionModelFactory.h"

#include <cmath>
#include <vector>

namespace sunspec {

void MpptInverterExtensionModelFactory::updateFromBuffer(std::optional<Model>& model,
                                                         const std::vector<uint16_t>& buffer,
                                                         uint32_t timestamp) {
    if (buffer.size() < 10) {
        return;
    }/* else if (buffer.size() != (10 + buffer.at(8) * 20)) {
        return {};
    }*/

    // Compute the number of MPPs from the buffer size (instead taking it from the payload).
    const auto count = (buffer.size()-10)/20;

    // If no model instance present, create one
    if (!model) {
        model.emplace(Model());
        model->m_modelId = buffer.front();
    }

    const double sfCurrent = pow(10.0, *(int16_t*)&buffer.at(2));
    const double sfVoltage = pow(10.0, *(int16_t*)&buffer.at(3));
    const double sfPower = pow(10.0, *(int16_t*)&buffer.at(4));

    std::vector<sunspec::Block<double>> dcs(count);
    for (uint16_t i = 0; i < count; ++i) {
        uint32_t dcCurrent = buffer.at(10 + (i * 20) + 9);
        if (dcCurrent == 65535) {
            dcCurrent = 0;
        }
        uint32_t dcVoltage = buffer.at(10 + (i * 20) + 10);
        if (dcVoltage == 65535) {
            dcVoltage = 0;
        }
        uint32_t dcPower = buffer.at(10 + (i * 20) + 11);
        if (dcPower == 65535) {
            dcPower = 0;
        }

        dcs[i][sunspec::current] = (int32_t)(dcCurrent * sfCurrent);
        dcs[i][sunspec::voltage] = (int32_t)(dcVoltage * sfVoltage);
        dcs[i][sunspec::power] = (int32_t)(dcPower * sfPower);
    }

    model.value().m_values[sunspec::timestamp] = timestamp;
    model.value().m_values[sunspec::dc] = dcs;
}

} // namespace sunspec
