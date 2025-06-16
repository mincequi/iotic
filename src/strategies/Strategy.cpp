#include "Strategy.h"

Strategy::Strategy(const std::string& thingId) :
    _thingId(thingId) {
}

Strategy::~Strategy() {
}

const std::string& Strategy::thingId() const {
    return _thingId;
}

json Strategy::toJson() const {
    return json();
}
