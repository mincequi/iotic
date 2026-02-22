#pragma once

#include <cstdint>
#include <variant>
#include <vector>

using DataPoint = std::variant<int16_t, std::vector<int16_t>>;
