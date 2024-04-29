#include <things/ThingValue.h>

using json = nlohmann::json;

ThingPropertyValue fromJsonValue(const nlohmann::json& value) {
    if (value.is_boolean()) return value.get<bool>();
    if (value.is_string()) return value.get<std::string>();
    return value.get<int>();
}

nlohmann::json toJsonValue(const ThingPropertyValue& value) {
    return std::visit([&](auto&& arg) {
        return json(arg);
    }, value);
}
