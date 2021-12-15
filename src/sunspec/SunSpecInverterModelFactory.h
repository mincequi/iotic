#ifndef SUNSPECINVERTERMODEL_H
#define SUNSPECINVERTERMODEL_H

#include "SunSpecModel.h"

namespace sunspec {

class InverterModelFactory {
public:
    static void updateFromBuffer(std::optional<Model>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECINVERTERMODEL_H
