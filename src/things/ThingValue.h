#pragma once

#include <nlohmann/json.hpp>
#include <uvw_iot/common/ThingProperty.h>

using uvw_iot::common::ThingPropertyValue;

ThingPropertyValue fromJsonValue(const nlohmann::json& value);
nlohmann::json toJsonValue(const ThingPropertyValue& value);

double toDouble(const ThingPropertyValue& value);
