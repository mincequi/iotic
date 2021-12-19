#include "SunSpecModelFactory.h"

#include <functional>
#include <map>

#include <sunspec/models/SunSpecInverterModelFactory.h>
#include <sunspec/models/SunSpecMpptInverterExtensionModelFactory.h>
#include <sunspec/models/SunSpecWyeConnectMeterModelFactory.h>

namespace sunspec {

static std::map<uint16_t, std::function<void(Model& model,
                                             const std::vector<uint16_t>& buffer,
                                             uint32_t timestamp)>> s_factories = {
    { 101, &InverterModelFactory::updateFromBuffer },
    { 103, &InverterModelFactory::updateFromBuffer },
    { 160, &MpptInverterExtensionModelFactory::updateFromBuffer },
    { 203, &WyeConnectMeterModelFactory::updateFromBuffer },
};

bool ModelFactory::updateFromBuffer(std::map<uint16_t, Model>& models,
                                    uint16_t modelId,
                                    const std::vector<uint16_t>& buffer,
                                    uint32_t timestamp) {
    if (buffer.empty() || s_factories.count(modelId) == 0) {
        return false;
    }

    if (models.count(modelId) == 0) {
        models[modelId].m_modelId = modelId;
    }

    s_factories.at(modelId)(models[modelId], buffer, timestamp);

    return true;
}

} // namespace sunspec
