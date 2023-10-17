#include "GoeUtil.h"

namespace goe {

ThingStatus toStatus(int v) {
    // api key "car"
    // carState, null if internal error (Unknown/Error=0, Idle=1, Charging=2, WaitCar=3, Complete=4, Error=5)
    switch (v) {
    case 0: return ThingStatus::unknown;
    case 1: return ThingStatus::idle;
    case 2: return ThingStatus::charging;
    case 3: return ThingStatus::waiting;
    case 4: return ThingStatus::waiting; //complete from goe is missleading here.
    case 5: return ThingStatus::error;
    default: return ThingStatus::unknown;
    }
}

} // namespace goe
