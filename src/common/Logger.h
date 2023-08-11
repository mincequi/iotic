#pragma once

#include <map>

#include <QString>

#include <loguru.hpp>
#include <magic_enum.hpp>

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
