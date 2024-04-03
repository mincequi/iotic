#pragma once

#include <array>
#include <ostream>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>

// TODO: remove int
using Value = std::variant<bool, int, double, std::string, std::array<double, 3>>;

template <typename T0, typename ... Ts>
std::ostream& operator<<(std::ostream& s, std::variant<T0, Ts...> const& v){
    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::array<double, 3>>) {
            s << "[" << arg[0] << ", " << arg[1] << ", " << arg[2] << "]";
        } else {
            s << arg;
        }
    }, v); return s;
}

Value fromJsonValue(const nlohmann::json& value);
nlohmann::json toJsonValue(const Value& value);

double toDouble(const Value& value);
