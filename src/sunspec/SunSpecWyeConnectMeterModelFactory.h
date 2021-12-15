#ifndef SUNSPECWYECONNECTMETERMODEL_H
#define SUNSPECWYECONNECTMETERMODEL_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "SunSpecDataPoint.h"
#include "SunSpecDataValue.h"
#include "SunSpecMeasuredValue.h"
#include "SunSpecModel.h"

namespace sunspec {

class WyeConnectMeterModelFactory {
public:
    static void updateFromBuffer(std::optional<Model>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECWYECONNECTMETERMODEL_H
