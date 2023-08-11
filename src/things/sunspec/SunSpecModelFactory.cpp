#include "SunSpecModelFactory.h"

#include <functional>
#include <map>

//#include <common/Logger.h>

#include <things/sunspec/SunSpecModel.h>
#include <things/sunspec/models/SunSpecCommonModelFactory.h>
#include <things/sunspec/models/SunSpecElgrisSmartMeterModelFactory.h>
#include <things/sunspec/models/SunSpecInverterModelFactory.h>
#include <things/sunspec/models/SunSpecMpptInverterExtensionModelFactory.h>
#include <things/sunspec/models/SunSpecWyeConnectMeterModelFactory.h>

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
    if (models[1].values().count(sunspec::Manufacturer) &&
            std::get<std::string>(models[1].values().at(sunspec::Manufacturer)) == "elgris") {
        ElgrisSmartMeterModelFactory::updateFromBuffer(models[modelId], buffer, timestamp);
    } else {
        s_factories.at(modelId)(models[modelId], buffer, timestamp);
    }

    return true;
}

} // namespace sunspec
