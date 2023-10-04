#pragma once

#include <map>

#include <QString>

#include <loguru.hpp>
#include <magic_enum.hpp>

#include <common/Util.h>

template <typename T0, typename ... Ts>
std::ostream& operator<<(std::ostream& s, std::variant<T0, Ts...> const& v){
    std::visit([&](auto && arg) { s << arg; }, v); return s;
}

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
