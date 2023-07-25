#ifndef SUNSPECDATAPOINT_H
#define SUNSPECDATAPOINT_H

#include <cstdint>

namespace sunspec {

enum DataPoint : uint8_t {
    // meta data
    timestamp,

    // measurement value
    //curr,
    min,
    max,

    // common model data points
    Manufacturer,
    Product,
    Options,
    Version,
    Serial,

    // regular data points
    totalActiveAcPower,
    totalExportedActiveEnergy,
    totalImportedActiveEnergy,
    operatingState,
    events,

    // blocks (collection of data points)
    dc,
    id,
    current,
    voltage,
    power
};

} // namespace sunspec

#endif // SUNSPECDATAPOINT_H
