#include "Formatters.h"

#include <cmath>
#include <sstream>

namespace fmt {

std::pair<std::string, std::string> toPower(double watts) {
    if (abs(watts) >= 10000) {
        int watts_ = std::round(watts/1000);
        return { std::to_string(watts_), "kW" };
    } else if (abs(watts) >= 1000) {
        double watts_ = std::round(watts/100.0) / 10.0;
        std::ostringstream out;
        out.setf(std::ios::fixed);
        out.precision(1);
        out << watts_;
        return { out.str(), "kW" };
    } else {
        int watts_ = std::round(watts/10.0) * 10;
        return { std::to_string(watts_), "W" };
    }
}

} // namespace fmt
