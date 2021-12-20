#include "SunSpecModelFactory.h"

#include <functional>
#include <map>

#include <sunspec/models/SunSpecCommonModel.h>
#include <sunspec/models/SunSpecElgrisSmartMeterModelFactory.h>
#include <sunspec/models/SunSpecInverterModelFactory.h>
#include <sunspec/models/SunSpecMpptInverterExtensionModelFactory.h>
#include <sunspec/models/SunSpecWyeConnectMeterModelFactory.h>

#include "Logger.h"

namespace sunspec {

static std::map<uint16_t, std::function<void(Model& model,
                                             const std::vector<uint16_t>& buffer,
                                             uint32_t timestamp)>> s_factories = {
    //{ 1,   &SunSpecCommonModelFactory::updateFromBuffer },
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

    // Special handling for elgris smart meter
    if (models[1].values().count(sunspec::manufacturer) &&
            std::get<std::string>(models[1].values().at(sunspec::manufacturer)) == "elgris") {
        ElgrisSmartMeterModelFactory::updateFromBuffer(models[modelId], buffer, timestamp);
    } else {
        s_factories.at(modelId)(models[modelId], buffer, timestamp);
    }

    return true;
}

} // namespace sunspec
