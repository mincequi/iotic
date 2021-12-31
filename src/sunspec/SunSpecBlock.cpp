#include "SunSpecBlock.h"

#include "SunSpecDataValue.h"
#include "SunSpecMeasuredValue.h"

namespace sunspec {

template <class T>
Block<T>::Block() {
}

template <class T>
template <class U>
Block<T>::Block(const Block<U>& other) {
    for (const auto& v : other.data()) {
        m_data[v.first] = v.second;
    }
}

template <class T>
bool Block<T>::operator==(const Block& other) const {
    return m_data == other.m_data;
}

template <class T>
bool Block<T>::operator!=(const Block& other) const {
    return m_data != other.m_data;
}

template <class T>
Block<T>& Block<T>::operator=(const Block& other) {
    for (const auto& kv : other.m_data) {
        m_data[kv.first] = kv.second;
    }

    return *this;
}

template <class T>
T& Block<T>::operator[](sunspec::DataPoint i) {
    return m_data[i];
}

template <class T>
const std::map<sunspec::DataPoint, T>& Block<T>::data() const {
    return m_data;
}

// Explicit template instantiation
template class Block<int32_t>;
template class Block<double>;
template class Block<MeasuredValue<int32_t>>;
template class Block<MeasuredValue<double>>;
template class Block<InverterOperatingState>;
template Block<MeasuredValue<double>>::Block(const Block<double>&);
template Block<MeasuredValue<InverterOperatingState>>::Block(const Block<InverterOperatingState>&);

} // namespace sunspec
