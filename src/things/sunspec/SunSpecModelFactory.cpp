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
                                             const std::vector<uint16_t>& buffer)>> s_factories = {
    //{ 1,   &SunSpecCommonModelFactory::updateFromBuffer },
    { Model::Id::InverterSinglePhase, &InverterModelFactory::updateFromBuffer },
    { Model::Id::InverterThreePhase, &InverterModelFactory::updateFromBuffer },
    { Model::Id::InverterMpptExtension, &MpptInverterExtensionModelFactory::updateFromBuffer },
    { Model::Id::MeterWyeConnectThreePhase, &WyeConnectMeterModelFactory::updateFromBuffer },
};

bool ModelFactory::updateFromBuffer(std::map<uint16_t, Model>& models,
                                    uint16_t modelId,
                                    const std::vector<uint16_t>& buffer) {
    if (buffer.empty() || s_factories.count(modelId) == 0) {
        return false;
    }

    if (models.count(modelId) == 0) {
        models[modelId]._modelId = modelId;
    }

    // Special handling for elgris smart meter
    if (models[1].values().count(sunspec::Manufacturer) &&
            std::get<std::string>(models[1].values().at(sunspec::Manufacturer)) == "elgris") {
        ElgrisSmartMeterModelFactory::updateFromBuffer(models[modelId], buffer);
    } else {
        s_factories.at(modelId)(models[modelId], buffer);
    }

    return true;
}

} // namespace sunspec
