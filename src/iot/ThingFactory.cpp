#include "ThingFactory.h"

#include <functional>
#include <map>

#include "goe/GoeCharger.h"
#include "ThingInfo.h"

static std::multimap<ThingInfo::Type, std::function<AbstractThingPtr(const ThingInfo&)>> _registry = {
    { ThingInfo::Type::Http, std::function(&GoeCharger::from) }
};

AbstractThingPtr ThingFactory::from(const ThingInfo& thingInfo) {
    auto itb = _registry.lower_bound(thingInfo.type());
    auto ite = _registry.upper_bound(thingInfo.type());
    for (auto it = itb; it != ite; ++it) {
        auto val = it->second(thingInfo);
        if (val) {
            return val;
        }
    }
    return nullptr;
}

ThingFactory::ThingFactory() {
}
