#pragma once

#include <map>

#include <QString>

#include <common/Logger.h>
#include <common/Util.h>

#include <iot/sunspec/SunSpecBlock.h>
#include <iot/sunspec/SunSpecDataPoint.h>
#include <iot/sunspec/SunSpecStatsValue.h>
#include <iot/sunspec/SunSpecMeasuredValue.h>
#include <iot/sunspec/SunSpecModel.h>
#include <iot/sunspec/SunSpecStatsModel.h>

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
