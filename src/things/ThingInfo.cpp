#include "ThingInfo.h"

ThingInfo::ThingInfo(DiscoveryType type, const std::string& id, const std::string& host)
    : _discoveryType(type),
      _id(id),
      _host(host) {
}

bool ThingInfo::isValid() const {
    return _discoveryType != DiscoveryType::Invalid && !_id.empty();
}

ThingInfo::DiscoveryType ThingInfo::discoveryType() const {
    return _discoveryType;
}

std::string ThingInfo::id() const {
    return _id;
}

std::string ThingInfo::host() const {
    return _host;
}
