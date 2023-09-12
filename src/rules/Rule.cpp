#include "Rule.h"

Rule::Rule(const std::string& thingId) :
    _thingId(thingId) {
}

Rule::~Rule() {
}

const std::string& Rule::thingId() const {
    return _thingId;
}
