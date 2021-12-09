#pragma once

#include <map>
#include <variant>

#include <loguru.hpp>

#include <QString>

#include "sunspec/SunSpecDataPoint.h"
#include "sunspec/SunSpecDataValue.h"
#include "sunspec/SunSpecMeasuredValue.h"

class Logger {
public:
    static void init(int& argc, char* argv[]);

private:
    Logger();
};

std::ostream& operator<<(std::ostream& os, const QString& qstr);

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec);

template <class T>
std::ostream& operator<<(std::ostream& os, const SunSpecMeasuredValue<T>& value);

template <typename T0, typename ... Ts>
std::ostream& operator<< (std::ostream& os, const std::variant<T0, Ts...>& v) {
    std::visit([&](auto && arg) { os << arg; }, v);
    return os;
}

std::ostream& operator<< (std::ostream& os, const std::map<sunspec::DataPoint, sunspec::DataValue>&);
