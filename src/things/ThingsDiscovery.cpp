#include "ThingsDiscovery.h"

ThingsDiscovery::ThingsDiscovery() {
}

ThingsDiscovery::~ThingsDiscovery() {
}

const dynamic_observable<ThingInfo> ThingsDiscovery::thingDiscovered() const {
    return _thingDiscovered.get_observable();
}
