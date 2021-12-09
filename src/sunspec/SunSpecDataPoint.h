#ifndef SUNSPECDATAPOINT_H
#define SUNSPECDATAPOINT_H

namespace sunspec {

enum DataPoint {
    curr,
    min,
    max,
    timestamp,
    totalActiveAcPower,

    totalExportedActiveEnergy,
    totalImportedActiveEnergy
};

} // namespace sunspec

#endif // SUNSPECDATAPOINT_H
