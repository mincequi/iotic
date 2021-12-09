#ifndef SUNSPECMODEL_H
#define SUNSPECMODEL_H

#include <variant>

#include "SunSpecCommonModel.h"
#include "SunSpecInverterModel.h"
#include "SunSpecMpptInverterExtensionModel.h"
#include "SunSpecWyeConnectMeterModel.h"

using SunSpecModel = std::variant<
    SunSpecCommonModel,
    SunSpecInverterModel,
    SunSpecMpptInverterExtensionModel,
    SunSpecWyeConnectMeterModel>;

#endif // SUNSPECMODEL_H
