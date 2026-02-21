#pragma once

#include <chrono>
#include <magic_enum.hpp>

namespace util {

template <typename E>
std::string toString(E value) {
    std::string out { magic_enum::enum_name(value) };
    if (!out.empty()) out[0] = std::tolower(out[0]);
    return out;
}

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

std::chrono::year_month_day parse_ymd(const std::string& s);

} // namespace util
