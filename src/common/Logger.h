#pragma once

#include <map>

#include <QString>

#include <magic_enum.hpp>
using namespace magic_enum::ostream_operators; // Define this before including loguru

#include <loguru.hpp>

#include <common/Util.h>

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
