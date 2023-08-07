#pragma once

#include <iot/sunspec/SunSpecBlock.h>
#include <iot/sunspec/SunSpecTypes.h>

namespace sunspec {

using LiveValue = std::variant<
    uint32_t,   // timestamp
    InverterOperatingState,
    InverterEvents,
    int32_t,    //
    double,     // regular value
    std::vector<Block<double>>,    // block arrays
    std::string>;

} // namespace sunspec
