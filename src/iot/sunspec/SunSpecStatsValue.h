#pragma once

#include <set>
#include <string>
#include <variant>
#include <vector>

#include <iot/sunspec/SunSpecLiveValue.h>
#include <iot/sunspec/SunSpecMeasuredValue.h>
#include <iot/sunspec/SunSpecStatsValue.h>
#include <iot/sunspec/SunSpecTypes.h>

namespace sunspec {

class StatsValue {
public:
    StatsValue& operator=(const LiveValue& v);
    StatsValue& operator|=(const InverterOperatingState& v);
    StatsValue& operator|=(const InverterEvents& v);

    bool isDirty() const;

    double minValue() const;
    double maxValue() const;

private:
    std::variant<
            uint32_t,
            std::set<InverterOperatingState>,
            InverterEvents,
            //MeasuredValue<int32_t>,
            MeasuredValue<double>,
            std::vector<Block<MeasuredValue<double>>>> m_variant;

    bool m_isDirty = false;

    friend std::ostream& operator<<(std::ostream& os, const sunspec::StatsValue& value);
    friend class StatsModel;
};

} // namespace sunspec
