#pragma once

#include <map>

#include <loguru.hpp>

#include <QString>

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
