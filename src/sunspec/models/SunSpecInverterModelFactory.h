#ifndef SUNSPECINVERTERMODEL_H
#define SUNSPECINVERTERMODEL_H

#include <sunspec/SunSpecModel.h>

namespace sunspec {

class InverterModelFactory {
public:
    static bool updateFromBuffer(Model& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECINVERTERMODEL_H
