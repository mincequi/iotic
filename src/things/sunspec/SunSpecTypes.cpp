#include "SunSpecTypes.h"

#include <magic_enum.hpp>

#include <things/sunspec/SunSpecMeasuredValue.h>

namespace sunspec {

std::ostream& operator<<(std::ostream& os, sunspec::InverterOperatingStatus state) {
    return os << "\"" << magic_enum::enum_name(state) << "\"";
}

std::ostream& operator<<(std::ostream& os, const InverterEvents& value) {
    os << "[";
    bool elementAdded = false;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value.test(i)) {
            auto e = magic_enum::enum_cast<InverterEvent>(i);
            if (e.has_value()) {
                elementAdded = true;
                os << "\"" << magic_enum::enum_name(e.value()) << "\",";
            }
        }
    }
    if (elementAdded) {
        os.seekp(-1, os.cur);
    }
    os << "]";
    return os;
}

} // namespace sunspec
