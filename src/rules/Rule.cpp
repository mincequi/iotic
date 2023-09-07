#include "Rule.h"

Rule::Rule(const std::string& id) :
    _id(id) {
}

Rule::~Rule() {
}


const std::string& Rule::id() const {
    return _id;
}
