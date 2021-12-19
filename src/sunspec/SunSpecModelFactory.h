#ifndef SUNSPECMODELFACTORY_H
#define SUNSPECMODELFACTORY_H

#include <cstdint>
#include <map>
#include <vector>

namespace sunspec {

class Model;

class ModelFactory {
public:
    static bool updateFromBuffer(std::map<uint16_t, Model>& models,
                                 uint16_t modelId,
                                 const std::vector<uint16_t>& buffer,
                                 uint32_t timestamp);
};

} // namespace sunspec

#endif // SUNSPECMODELFACTORY_H
