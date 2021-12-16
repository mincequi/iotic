#ifndef SUNSPECWYECONNECTMETERMODEL_H
#define SUNSPECWYECONNECTMETERMODEL_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <sunspec/SunSpecDataPoint.h>
#include <sunspec/SunSpecDataValue.h>
#include <sunspec/SunSpecMeasuredValue.h>
#include <sunspec/SunSpecModel.h>

namespace sunspec {

class WyeConnectMeterModelFactory {
public:
    static void updateFromBuffer(std::optional<Model>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECWYECONNECTMETERMODEL_H
