#ifndef SUNSPECDATAVALUE_H
#define SUNSPECDATAVALUE_H

#include <variant>

#include "SunSpecMeasuredValue.h"

namespace sunspec {

using DataValue = std::variant<uint32_t, SunSpecMeasuredValue<int32_t>, SunSpecMeasuredValue<uint32_t>>;

} // namespace sunspec

#endif // SUNSPECDATAVALUE_H
