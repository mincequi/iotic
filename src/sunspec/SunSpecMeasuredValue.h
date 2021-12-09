#ifndef SUNSPECMEASUREDVALUE_H
#define SUNSPECMEASUREDVALUE_H

#include <algorithm>
#include <map>
#include <ostream>

#include "SunSpecDataPoint.h"

template <class T>
class SunSpecMeasuredValue {
public:
    SunSpecMeasuredValue() {}

    SunSpecMeasuredValue(T value) {
        *this = value;
    }

    SunSpecMeasuredValue& operator=(const SunSpecMeasuredValue& other) {
        *this = other(sunspec::curr);
        return *this;
    }

    SunSpecMeasuredValue& operator=(T value) {
        if (m_values.empty()) {
            m_values[sunspec::curr] = value;
            m_values[sunspec::min] = value;
            m_values[sunspec::max] = value;
            return *this;
        }

        m_values[sunspec::curr] = value;
        m_values[sunspec::min] = std::min(m_values[sunspec::min], value);
        m_values[sunspec::max] = std::max(m_values[sunspec::max], value);
        return *this;
    }

    T operator()(sunspec::DataPoint dp) const {
        return m_values.at(dp);
    }

    const std::map<sunspec::DataPoint, T>& data() const {
        return m_values;
    }

private:
    std::map<sunspec::DataPoint, T> m_values;
};

#endif // SUNSPECMEASUREDVALUE_H
