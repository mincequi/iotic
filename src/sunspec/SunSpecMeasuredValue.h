#ifndef SUNSPECMEASUREDVALUE_H
#define SUNSPECMEASUREDVALUE_H

#include <algorithm>

#include "SunSpecBlock.h"
#include "SunSpecDataPoint.h"

namespace sunspec {

template <class T>
class SunSpecMeasuredValue : public sunspec::Block<T> {
public:
    SunSpecMeasuredValue() {}
    SunSpecMeasuredValue(T value);

    SunSpecMeasuredValue& operator=(const SunSpecMeasuredValue& other);
    SunSpecMeasuredValue& operator=(T value);

    T operator()(sunspec::DataPoint dp) const;

    bool isDirty() const;

private:
    bool m_isDirty = true;
};

} // namespace sunspec

#endif // SUNSPECMEASUREDVALUE_H
