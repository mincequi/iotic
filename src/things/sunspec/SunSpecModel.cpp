#include "SunSpecModel.h"

namespace sunspec {

uint16_t Model::modelId() const {
    return _modelId;
}

const std::map<DataPoint, LiveValue>& Model::values() const {
    return _values;
}

bool Model::operator==(const Model& other) const {
    return values() == other.values();
}

bool Model::operator!=(const Model& other) const {
    return values() != other.values();
}

LiveValue& Model::operator[](DataPoint i) {
    return _values[i];
}

} // namespace sunspec
