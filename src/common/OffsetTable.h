#pragma once

#include <limits>
#include <vector>

static const std::vector<int> offsetTable = {
    std::numeric_limits<int>::min(),
    -400,
    -250,
    -150,
    0,
    150,
    250,
    400,
    600,
    1000,
    1500,
    2500,
    4000,
    6000,
    10000,
    // 15000
    // 25000
};
