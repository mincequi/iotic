#pragma once

#include <map>

#include <QString>

#include <common/Logger.h>

#include <iot/sunspec/SunSpecBlock.h>
#include <iot/sunspec/SunSpecDataPoint.h>
#include <iot/sunspec/SunSpecDataValue.h>
#include <iot/sunspec/SunSpecMeasuredValue.h>
#include <iot/sunspec/SunSpecModel.h>

namespace sunspec {
class StatsModel;
}

namespace sunspec {

std::ostream& operator<<(std::ostream& os, const sunspec::Model& value);

template <typename T0, typename ... Ts>
std::ostream& operator<< (std::ostream& os, const std::variant<T0, Ts...>& v) {
    std::visit([&](auto&& arg) { os << arg; }, v);
    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const sunspec::Block<T>& value);

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set);

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec);

std::ostream& operator<<(std::ostream& os, const sunspec::StatsModel& value);

std::ostream& operator<<(std::ostream& os, const sunspec::StatsValue& value);

//template <class T>
//std::ostream& operator<<(std::ostream& os, const SunSpecMeasuredValue<T>& value);


} // namespace sunspec
