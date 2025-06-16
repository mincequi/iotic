#include "RuleUtil.h"

#include <common/Util.h>

#include <tinyexpr.h>

namespace rule {

json toJson(const te_parser& teParser) {
    json j;

    for (const auto& v : teParser.get_variables_and_functions()) {
        std::visit(util::overload{
                       [&](double d) { j[v.m_name] = d; },
                       [&](const double* d) { j[v.m_name] = *d; },
                       [](auto&&) { }
                   }, v.m_value);
    }

    return j;
}

}
