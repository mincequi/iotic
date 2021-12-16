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

    // regular data points
    totalActiveAcPower,
    totalExportedActiveEnergy,
    totalImportedActiveEnergy,
    operatingState,
    events,

    // blocks (collection of data points)
    dc,
    current,
    voltage,
    power
};

} // namespace sunspec

#endif // SUNSPECDATAPOINT_H
