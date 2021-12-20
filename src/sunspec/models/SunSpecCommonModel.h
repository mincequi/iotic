#ifndef SUNSPECCOMMONMODEL_H
#define SUNSPECCOMMONMODEL_H

#include <optional>
#include <string>
#include <vector>

#include <sunspec/SunSpecModel.h>

namespace sunspec {

class SunSpecCommonModelFactory : public sunspec::Model {
public:
    static bool updateFromBuffer(Model& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);

private:
    static std::string readString(const uint16_t* begin, uint8_t length);
};

} // namespace sunspec

#endif // SUNSPECCOMMONMODEL_H
