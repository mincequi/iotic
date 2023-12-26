#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sunspec {
class Model;
}

class SunSpecCommonModelFactory {
public:
    static bool updateFromBuffer(sunspec::Model& model, const std::vector<uint16_t>& buffer);

private:
    static std::string readString(const uint16_t* begin, uint8_t length);
};
