#ifndef SUNSPECMPPTINVERTEREXTENSIONMODEL_H
#define SUNSPECMPPTINVERTEREXTENSIONMODEL_H

#include <optional>
#include <string>
#include <vector>

#include "SunSpecDataPoint.h"
#include "SunSpecDataValue.h"

class SunSpecMpptInverterExtensionModel {
public:
    static void updateFromBuffer(std::optional<SunSpecMpptInverterExtensionModel>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);

    void updateFromBuffer(const std::vector<uint16_t>& buffer);

    std::vector<SunSpecMeasuredValue<uint32_t>> dcPowers;  // Watts
    uint32_t timestamp = 0;

    const std::map<sunspec::DataPoint, sunspec::DataValue>& values() const;

private:
    SunSpecMpptInverterExtensionModel();

    std::map<sunspec::DataPoint, sunspec::DataValue> m_values;
};

#endif // SUNSPECMPPTINVERTEREXTENSIONMODEL_H
