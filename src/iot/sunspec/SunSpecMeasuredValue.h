#ifndef SUNSPECMEASUREDVALUE_H
#define SUNSPECMEASUREDVALUE_H

#include <algorithm>

#include "SunSpecBlock.h"
#include "SunSpecDataPoint.h"

namespace sunspec {

template <class T>
class MeasuredValue : public sunspec::Block<T> {
public:
    MeasuredValue() {}
    MeasuredValue(T value);

    MeasuredValue& operator=(const MeasuredValue& other);
    MeasuredValue& operator=(T value);

    T operator()(sunspec::DataPoint dp) const;

    bool isDirty() const;

private:
    bool m_isDirty = true;
};

} // namespace sunspec

#endif // SUNSPECMEASUREDVALUE_H
