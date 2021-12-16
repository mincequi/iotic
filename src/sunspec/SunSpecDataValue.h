#ifndef SUNSPECDATAVALUE_H
#define SUNSPECDATAVALUE_H

#include <variant>
#include <vector>

#include "SunSpecMeasuredValue.h"
#include "SunSpecTypes.h"

namespace sunspec {

using LiveValue = std::variant<
    uint32_t,   // timestamp
    InverterOperatingState,
    InverterEvents,
    int32_t,    //
    double,     // regular value
    std::vector<Block<double>>>;    // block arrays

class StatsValue : public std::variant<
        uint32_t,
        MeasuredValue<InverterOperatingState>,
        InverterEvents,
        MeasuredValue<int32_t>,
        MeasuredValue<double>,
        std::vector<Block<MeasuredValue<double>>>> {
    using variant::variant;

public:
    StatsValue& operator=(const LiveValue& v);
    StatsValue& operator|=(const StatsValue& v);

    bool isDirty() const;

private:
    bool m_isDirty = true;
};

} // namespace sunspec

#endif // SUNSPECDATAVALUE_H
