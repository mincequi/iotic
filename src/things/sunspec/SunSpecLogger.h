#pragma once

#include <map>

#include <common/Logger.h>
#include <common/Util.h>

#include <things/sunspec/SunSpecBlock.h>
#include <things/sunspec/SunSpecDataPoint.h>
#include <things/sunspec/SunSpecStatsValue.h>
#include <things/sunspec/SunSpecMeasuredValue.h>
#include <things/sunspec/SunSpecModel.h>
#include <things/sunspec/SunSpecStatsModel.h>

namespace sunspec {

template <typename T0, typename ... Ts>
std::ostream& operator<< (std::ostream& os, const std::variant<T0, Ts...>& v) {
    std::visit([&](auto&& arg) { os << arg; }, v);
    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec);
std::ostream& operator<<(std::ostream& os, const sunspec::StatsModel& model);
std::ostream& operator<<(std::ostream& os, const sunspec::Model& model);
std::ostream& operator<<(std::ostream& os, const sunspec::StatsValue& value);
template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set);
template <class T>
std::ostream& operator<<(std::ostream& os, const sunspec::Block<T>& value);

} // namespace sunspec
