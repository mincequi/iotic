#pragma once

#include <cstdint>
#include <vector>

namespace sunspec {

class Model;

class MpptInverterExtensionModelFactory {
public:
    static bool updateFromBuffer(Model& model,
                                 const std::vector<uint16_t>& buffer);
};

} // namespace sunspec
