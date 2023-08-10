#include "ThingInfo.h"

ThingInfo::ThingInfo(Type type, const std::string& id)
    : _type(type),
      _id(id) {
}

bool ThingInfo::isValid() const {
    return _type != Type::Invalid && !_id.empty();
}

ThingInfo::Type ThingInfo::type() const {
    return _type;
}

std::string ThingInfo::id() const {
    return _id;
}

std::string ThingInfo::name() const {
    return _name.empty() ? _id : _name;
}
