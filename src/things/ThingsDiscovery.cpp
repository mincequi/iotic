#include "ThingsDiscovery.h"

ThingsDiscovery::ThingsDiscovery() {
}

ThingsDiscovery::~ThingsDiscovery() {
}

const dynamic_observable<ThingPtr> ThingsDiscovery::thingDiscovered() const {
    return _thingDiscovered.get_observable();
}

dynamic_subscriber<ThingPtr> ThingsDiscovery::thingDiscoveredSubscriber() const {
    return _thingDiscovered.get_subscriber().as_dynamic();
}
