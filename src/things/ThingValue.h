#pragma once

#include <nlohmann/json.hpp>
#include <uvw_iot/ThingProperty.h>

using uvw_iot::ThingPropertyValue;

ThingPropertyValue fromJsonValue(const nlohmann::json& value);
nlohmann::json toJsonValue(const ThingPropertyValue& value);

double toDouble(const ThingPropertyValue& value);
