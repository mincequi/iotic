#include "HttpThingFactory.h"

#include <functional>
#include <map>

#include <things/ThingInfo.h>
#include <things/goe/GoeCharger.h>
#include <things/shelly/Shelly.h>

// We can have multiple factories per type, so this is a multimap
static std::multimap<ThingInfo::DiscoveryType, std::function<ThingPtr(const ThingInfo&)>> _registry = {
    { ThingInfo::DiscoveryType::Http, std::function(&GoeCharger::from) },
    { ThingInfo::DiscoveryType::Http, std::function(&Shelly::from) }
};

ThingPtr HttpThingFactory::from(const ThingInfo& thingInfo) {
    // Test each factory for specific type. First factory that provides a thing is accepted.
    auto itb = _registry.lower_bound(thingInfo.discoveryType());
    auto ite = _registry.upper_bound(thingInfo.discoveryType());
    for (auto it = itb; it != ite; ++it) {
        auto val = it->second(thingInfo);
        if (val) {
            return val;
        }
    }
    return nullptr;
}

HttpThingFactory::HttpThingFactory() {
}
