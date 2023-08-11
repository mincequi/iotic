#pragma once

#include <map>

#include "SunSpecDataPoint.h"

namespace sunspec {

template <class T>
class Block {
public:
    Block();

    template <class U>
    Block(const Block<U>& other);

    bool operator==(const Block& other) const;
    bool operator!=(const Block& other) const;

    Block& operator=(const Block& other);

    T& operator[](sunspec::DataPoint i);

    const std::map<sunspec::DataPoint, T>& data() const;

protected:
    std::map<sunspec::DataPoint, T> m_data;
};

} // namespace sunspec
