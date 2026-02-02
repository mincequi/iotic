#include <things/ThingValue.h>

using json = nlohmann::json;

ThingPropertyValue fromJsonValue(const json& j) {
    if (j.is_boolean()) return j.get<bool>();
    if (j.is_number_integer()) return j.get<int>();
    if (j.is_string()) return j.get<std::string>();
    if (j.is_array()) {
        if (j.size() == 3 && j[0].is_number_integer())
            return j.get<std::array<int, 3>>();

        return j.get<std::vector<bool>>();
    }

    return j.get<int>();
}

nlohmann::json toJsonValue(const ThingPropertyValue& value) {
    return std::visit([&](auto&& arg) {
        return json(arg);
    }, value);
}
