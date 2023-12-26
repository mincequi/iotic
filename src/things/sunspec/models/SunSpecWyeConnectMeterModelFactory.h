#pragma once

#include <cstdint>
#include <vector>

namespace sunspec {

class Model;

class WyeConnectMeterModelFactory {
public:
    static bool updateFromBuffer(Model& model,
                                 const std::vector<uint16_t>& buffer);
};

} // namespace sunspec
