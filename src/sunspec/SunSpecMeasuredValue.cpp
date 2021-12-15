#include "SunSpecMeasuredValue.h"

namespace sunspec {

template <class T>
SunSpecMeasuredValue<T>::SunSpecMeasuredValue(T value) {
    *this = value;
}

template <class T>
SunSpecMeasuredValue<T>& SunSpecMeasuredValue<T>::operator=(const SunSpecMeasuredValue& other) {
    // Other will always be an initial value, so it's ok, to use min here.
    *this = other(sunspec::min);
    return *this;
}

template <class T>
SunSpecMeasuredValue<T>& SunSpecMeasuredValue<T>::operator=(T value) {
    if (sunspec::Block<T>::m_data.empty()) {
        //sunspec::Block<T>::m_data[sunspec::curr] = value;
        sunspec::Block<T>::m_data[sunspec::min] = value;
        sunspec::Block<T>::m_data[sunspec::max] = value;
        return *this;
    }

    m_isDirty = false;
    //sunspec::Block<T>::m_data[sunspec::curr] = value;
    const auto min = std::min(sunspec::Block<T>::m_data[sunspec::min], value);
    if (sunspec::Block<T>::m_data[sunspec::min] != min) {
        sunspec::Block<T>::m_data[sunspec::min] = min;
        m_isDirty = true;
    }
    const auto max = std::max(sunspec::Block<T>::m_data[sunspec::max], value);
    if (sunspec::Block<T>::m_data[sunspec::max] != max) {
        sunspec::Block<T>::m_data[sunspec::max] = max;
        m_isDirty = true;
    }

    return *this;
}

template <class T>
T SunSpecMeasuredValue<T>::operator()(sunspec::DataPoint dp) const {
    return sunspec::Block<T>::m_data.at(dp);
}

template <class T>
bool SunSpecMeasuredValue<T>::isDirty() const {
    return m_isDirty;
}

// Explicit template instantiation
template class SunSpecMeasuredValue<int32_t>;
template class SunSpecMeasuredValue<double>;

} // namespace sunspec
