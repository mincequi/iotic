#include "Util.h"

#include <regex>

using namespace std::chrono;

namespace util {

year_month_day parse_ymd(const std::string& s) {
    static const std::regex pattern(R"(^(\d{4})-(\d{2})-(\d{2})$)");

    std::smatch match;
    if (!std::regex_match(s, match, pattern))
        return {};

    int y = std::stoi(match[1].str());
    unsigned m = std::stoi(match[2].str());
    unsigned d = std::stoi(match[3].str());

    year_month_day ymd{
        year{y},
        month{m},
        day{d}
    };

    return ymd;
}

} // namespace util
