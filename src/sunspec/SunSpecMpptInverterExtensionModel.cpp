#include "SunSpecMpptInverterExtensionModel.h"

#include <cmath>

void SunSpecMpptInverterExtensionModel::updateFromBuffer(std::optional<SunSpecMpptInverterExtensionModel>& model,
                                                         const std::vector<uint16_t>& buffer,
                                                         uint32_t timestamp) {
    if (buffer.size() < 10) {
        return;
    }/* else if (buffer.size() != (10 + buffer.at(8) * 20)) {
        return {};
    }*/

    // Compute the number of MPPs from the buffer size (instead taking it from the payload).
    const auto count = (buffer.size()-10)/20;

    SunSpecMpptInverterExtensionModel out;
    const int16_t sunssf = *(int16_t*)&buffer.at(4);
    std::vector<uint32_t> dcPowers(count);

    for (uint16_t i = 0; i < count; ++i) {
        uint32_t value = buffer.at(10 + (i * 20) + 11);
        if (value == 65535) {
            value = 0;
        }

        dcPowers[i] = value * pow(10.0, sunssf);
    }

    if (!model) {
        model.emplace(SunSpecMpptInverterExtensionModel());
        return;
    }

    model.value().timestamp = timestamp;
    model.value().dcPowers.resize(count);
    for (size_t i = 0; i < count; ++i) {
        model.value().dcPowers[i] = dcPowers[i];
    }
}

const std::map<sunspec::DataPoint, sunspec::DataValue>& SunSpecMpptInverterExtensionModel::values() const {
    return m_values;
}

SunSpecMpptInverterExtensionModel::SunSpecMpptInverterExtensionModel() {
}
