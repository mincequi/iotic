#include <things/ThingValue.h>

using json = nlohmann::json;

Value fromJsonValue(const nlohmann::json& value) {
    if (value.is_boolean()) return value.get<bool>();
    if (value.is_string()) return value.get<std::string>();
    return value.get<double>();
}

nlohmann::json toJsonValue(const Value& value) {
    return std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>)
                return json(arg);
        else if constexpr (std::is_same_v<T, std::array<double, 3>>)
                return json(arg);
        else return json(arg);
    }, value);
}

double toDouble(const Value& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    return 0.0;
}
