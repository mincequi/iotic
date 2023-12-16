#include "ThingsDiscovery.h"

ThingsDiscovery::ThingsDiscovery() {
}

ThingsDiscovery::~ThingsDiscovery() {
}

void ThingsDiscovery::start(int) {
}

void ThingsDiscovery::stop() {
}

const dynamic_observable<ThingPtr> ThingsDiscovery::thingDiscovered() const {
    return _thingDiscovered.get_observable();
}

dynamic_subscriber<ThingPtr> ThingsDiscovery::thingDiscoveredSubscriber() const {
    return _thingDiscovered.get_subscriber().as_dynamic();
}
