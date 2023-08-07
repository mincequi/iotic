#include "SunSpecMeasuredValue.h"

#include "SunSpecStatsValue.h"

namespace sunspec {

template <class T>
MeasuredValue<T>::MeasuredValue(T value) {
    *this = value;
}

template <class T>
MeasuredValue<T>& MeasuredValue<T>::operator=(const MeasuredValue& other) {
    // Other will always be an initial value, so it's ok, to use min here.
    *this = other(sunspec::min);
    return *this;
}

template <class T>
MeasuredValue<T>& MeasuredValue<T>::operator=(T value) {
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
T MeasuredValue<T>::operator()(sunspec::DataPoint dp) const {
    return sunspec::Block<T>::m_data.at(dp);
}

template <class T>
bool MeasuredValue<T>::isDirty() const {
    return m_isDirty;
}

// Explicit template instantiation
template class MeasuredValue<int32_t>;
template class MeasuredValue<double>;
template class MeasuredValue<InverterOperatingState>;

} // namespace sunspec
