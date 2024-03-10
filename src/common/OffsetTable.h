#pragma once

#include <limits>
#include <vector>

static const int defaultOffset = 3;

static const std::vector<int> offsetTable = { // 6th root of ten
    std::numeric_limits<int16_t>::min(),
    -500,
    //-400,
    //-300,
    -200,
    //-150,
    //-100,
    0,
    //100,
    //150,
    200,
    //300,
    //400,
    500,
    //700,
    1000,
    //1500, // this prevents from switching off (acutally not)
    2000, // this prevents from switching down to off - with some extra headroom
    //3000, // this prevents phase switching up to 3
    5000, // this prevents phase switching down to 1
    //7000,
    10000,
    // 15000
    //20000
    std::numeric_limits<int16_t>::max(),
};
