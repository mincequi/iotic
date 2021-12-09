#ifndef SUNSPECWYECONNECTMETERMODEL_H
#define SUNSPECWYECONNECTMETERMODEL_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "SunSpecDataPoint.h"
#include "SunSpecDataValue.h"
#include "SunSpecMeasuredValue.h"

class SunSpecWyeConnectMeterModel {
public:
    static void updateFromBuffer(std::optional<SunSpecWyeConnectMeterModel>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);

    const std::map<sunspec::DataPoint, sunspec::DataValue>& values() const;

private:
    SunSpecWyeConnectMeterModel();

    std::map<sunspec::DataPoint, sunspec::DataValue> m_values;
};

#endif // SUNSPECWYECONNECTMETERMODEL_H
