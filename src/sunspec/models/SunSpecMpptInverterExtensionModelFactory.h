#ifndef SUNSPECMPPTINVERTEREXTENSIONMODEL_H
#define SUNSPECMPPTINVERTEREXTENSIONMODEL_H

#include <cstdint>
#include <optional>
#include <vector>

namespace sunspec {

class Model;

class MpptInverterExtensionModelFactory {
public:
    static void updateFromBuffer(std::optional<Model>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECMPPTINVERTEREXTENSIONMODEL_H
