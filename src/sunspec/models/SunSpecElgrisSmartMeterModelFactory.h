#ifndef SUNSPECSMARTMETERMODEL_H
#define SUNSPECSMARTMETERMODEL_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <sunspec/SunSpecDataPoint.h>
#include <sunspec/SunSpecDataValue.h>
#include <sunspec/SunSpecMeasuredValue.h>
#include <sunspec/SunSpecModel.h>

namespace sunspec {

class ElgrisSmartMeterModelFactory {
public:
    static bool updateFromBuffer(Model& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECSMARTMETERMODEL_H
