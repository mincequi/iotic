#pragma once

#include <cstdint>

namespace sunspec {

enum DataPoint {
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
    operatingStatus,
    events,

    // blocks (collection of data points)
    dc,
    id,
    current,
    voltage,
    power
};

} // namespace sunspec
