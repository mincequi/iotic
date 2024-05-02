#pragma once

#include <iostream>

#include <uvw_iot/ThingProperty.h>

#include <magic_enum_iostream.hpp>
using magic_enum::iostream_operators::operator<<; // Define this before including loguru
std::ostream& operator<<(std::ostream& s, const uvw_iot::ThingPropertyValue& value);
std::ostream& operator<<(std::ostream& s, const uvw_iot::ThingPropertyMap& map);

#include <loguru.hpp>

class Logger {
public:
    static void init(int& argc, char* argv[]);

private:
    Logger();
};
