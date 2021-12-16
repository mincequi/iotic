#pragma once

#include <map>

#include <loguru.hpp>

#include <QString>

#include "sunspec/SunSpecBlock.h"
#include "sunspec/SunSpecDataPoint.h"
#include "sunspec/SunSpecDataValue.h"
#include "sunspec/SunSpecMeasuredValue.h"
#include "sunspec/SunSpecModel.h"

namespace sunspec {
class StatsModel;
}

class Logger {
public:
    static void init(int& argc, char* argv[]);

private:
    Logger();
};

std::ostream& operator<<(std::ostream& os, const QString& qstr);


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

//template <class T>
//std::ostream& operator<<(std::ostream& os, const SunSpecMeasuredValue<T>& value);


} // namespace sunspec
