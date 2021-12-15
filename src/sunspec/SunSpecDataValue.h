#ifndef SUNSPECDATAVALUE_H
#define SUNSPECDATAVALUE_H

#include <variant>
#include <vector>

#include "SunSpecMeasuredValue.h"

namespace sunspec {

using LiveValue = std::variant<
    uint32_t,   // timestamp
    int32_t,    //
    double,     // regular value
    std::vector<Block<double>>>;    // block arrays

using StatsValue = std::variant<
    uint32_t,
    SunSpecMeasuredValue<int32_t>,
    SunSpecMeasuredValue<double>,
    std::vector<Block<SunSpecMeasuredValue<double>>>>;

} // namespace sunspec

#endif // SUNSPECDATAVALUE_H
