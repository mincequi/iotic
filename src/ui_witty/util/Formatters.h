#pragma once

#include <tuple>

namespace fmt {

/// [value, unit]
std::pair<std::string, std::string> toPower(double watts);

}
