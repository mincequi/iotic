#ifndef SUNSPECMPPTINVERTEREXTENSIONMODEL_H
#define SUNSPECMPPTINVERTEREXTENSIONMODEL_H

#include "SunSpecModel.h"

namespace sunspec {

class MpptInverterExtensionModelFactory {
public:
    static void updateFromBuffer(std::optional<Model>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECMPPTINVERTEREXTENSIONMODEL_H
