#ifndef SUNSPECCOMMONMODEL_H
#define SUNSPECCOMMONMODEL_H

#include <cstdint>
#include <string>
#include <vector>

namespace sunspec {

class Model;

class SunSpecCommonModelFactory {
public:
    static bool updateFromBuffer(Model& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);

private:
    static std::string readString(const uint16_t* begin, uint8_t length);
};

} // namespace sunspec

#endif // SUNSPECCOMMONMODEL_H
