#pragma once

#include <things/sunspec/SunSpecBlock.h>
#include <things/sunspec/SunSpecTypes.h>

namespace sunspec {

using LiveValue = std::variant<
    uint32_t,   // timestamp
    InverterOperatingStatus,
    InverterEvents,
    int32_t,    //
    double,     // regular value
    std::vector<Block<double>>,    // block arrays
    std::string>;

} // namespace sunspec
