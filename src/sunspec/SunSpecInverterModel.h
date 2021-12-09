#ifndef SUNSPECINVERTERMODEL_H
#define SUNSPECINVERTERMODEL_H

#include <optional>
#include <vector>

#include "SunSpecDataPoint.h"
#include "SunSpecDataValue.h"
#include "SunSpecMeasuredValue.h"

class SunSpecInverterModel {
public:
    static void updateFromBuffer(std::optional<SunSpecInverterModel>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);

    const std::map<sunspec::DataPoint, sunspec::DataValue>& values() const;

private:
    SunSpecInverterModel();

    std::map<sunspec::DataPoint, sunspec::DataValue> m_values;
};

#endif // SUNSPECINVERTERMODEL_H
