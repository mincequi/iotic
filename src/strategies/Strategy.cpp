#include "Strategy.h"

Strategy::Strategy(const std::string& thingId) :
    _thingId(thingId) {
}

Strategy::~Strategy() {
}

json Strategy::toJson() const {
    return json();
}
