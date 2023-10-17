#pragma once

#include <limits>
#include <vector>

static const std::vector<int> offsetTable = { // 6th root of ten
    std::numeric_limits<int16_t>::min(),
    -500,
    //-400,
    -300,
    -200,
    //-150,
    //-100,
    0,
    //100,
    //150,
    200,
    300,
    //400,
    500,
    700,
    1000,
    1500, // this prevents from switching off
    2000, // this prevents from switching off - with some extra headroom
    3000, // this prevents phase switching to 3
    5000, // this prevents phase switching to 1
    7000,
    10000,
    // 15000
    // 20000
};

static const std::vector<int> alt2OffsetTable = { // 10th root of ten
    std::numeric_limits<int16_t>::min(),
    -500,
    -400,
    -300,
    //-250,
    -200,
    //-150,
    //-120,
    //-100,
    0,
    //100,
    //120,
    //150,
    200,
    //250,
    300,
    400,
    500,
    600,
    800,

    1000,
    1200,
    1500,
    2000,
    2500,
    3000,
    4000,
    5000,
    6000,
    8000,

    10000,
    12000,
    15000,
    20000,
    25000
};
