#pragma once

#include <cstdint>
#include <vector>

namespace sunspec {

class Model;

class ElgrisSmartMeterModelFactory {
public:
    static bool updateFromBuffer(Model& model, const std::vector<uint16_t>& buffer);
};

} // namespace sunspec
