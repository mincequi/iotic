#ifndef SUNSPECMODELFACTORY_H
#define SUNSPECMODELFACTORY_H

#include <cstdint>
#include <optional>
#include <vector>

namespace sunspec {

class Model;

class ModelFactory {
public:
    static void updateFromBuffer(std::optional<Model>& model,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECMODELFACTORY_H
