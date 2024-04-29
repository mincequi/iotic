#pragma once

#include <iostream>

#include <uvw_iot/common/ThingProperty.h>

#include <magic_enum_iostream.hpp>
using magic_enum::iostream_operators::operator<<; // Define this before including loguru
std::ostream& operator<<(std::ostream& s, const uvw_iot::common::ThingPropertyValue& value);
std::ostream& operator<<(std::ostream& s, const uvw_iot::common::ThingPropertyMap& map);

#include <loguru.hpp>

class Logger {
public:
    static void init(int& argc, char* argv[]);

private:
    Logger();
};
