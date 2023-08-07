#ifndef SUNSPECSTATSMODEL_H
#define SUNSPECSTATSMODEL_H

#include <map>

#include "SunSpecDataPoint.h"
#include "SunSpecStatsValue.h"
#include "SunSpecTypes.h"

namespace sunspec {

class Model;

class StatsModel {
public:
    StatsModel();

    StatsModel& operator=(const Model& model);

    uint16_t modelId() const;

    bool isDirty() const;

    const std::map<DataPoint, StatsValue>& values() const;

private:
    uint16_t m_modelId = 0;
    bool m_isDirty = false;
    std::map<DataPoint, StatsValue> m_values;
};

} // namespace sunspec

#endif // SUNSPECSTATSMODEL_H
