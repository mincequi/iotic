#include "ThingInfo.h"

#include <algorithm>
#include <regex>

ThingInfo::ThingInfo(DiscoveryType type, const std::string& id, const std::string& host)
    : _discoveryType(type),
      _id(id),
      _host(host) {
    std::transform(_id.begin(), _id.end(), _id.begin(), [](auto c) { return std::tolower(c); });
    _id = std::regex_replace(_id, std::regex("[^a-z0-9]"), "_");
}

ThingInfo::~ThingInfo() {
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
