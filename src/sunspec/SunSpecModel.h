#ifndef SUNSPECMODEL_H
#define SUNSPECMODEL_H

#include <map>

#include "SunSpecDataPoint.h"
#include "SunSpecDataValue.h"

namespace sunspec {

class Model {
public:
    bool operator==(const Model& other) const;
    bool operator!=(const Model& other) const;

    uint16_t modelId() const;

    const std::map<DataPoint, LiveValue>& values() const;
    LiveValue& operator[](DataPoint i);

protected:
    uint16_t m_modelId = 0;

    std::map<DataPoint, LiveValue> m_values;

    friend class InverterModelFactory;
    friend class MpptInverterExtensionModelFactory;
    friend class WyeConnectMeterModelFactory;
};

} // namespace sunspec

#endif // SUNSPECMODEL_H
