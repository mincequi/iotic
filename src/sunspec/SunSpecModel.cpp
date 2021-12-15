#include "SunSpecModel.h"

namespace sunspec {

uint16_t Model::modelId() const {
    return m_modelId;
}

const std::map<DataPoint, LiveValue>& Model::values() const {
    return m_values;
}

bool Model::operator==(const Model& other) const {
    return values() == other.values();
}

bool Model::operator!=(const Model& other) const {
    return values() != other.values();
}

LiveValue& Model::operator[](DataPoint i) {
    return m_values[i];
}

} // namespace sunspec
