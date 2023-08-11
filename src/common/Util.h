#pragma once

#include <QString>

#include <magic_enum.hpp>

namespace util {

QString getMacAddress();

template <typename E>
std::string toString(E value) {
    std::string out { magic_enum::enum_name(value) };
    if (!out.empty()) out[0] = std::tolower(out[0]);
    return out;
}

} // namespace util
