#include "AbstractDiscovery.h"

#include <common/Logger.h>
#include <iot/ThingFactory.h>
#include <iot/ThingManager.h>

AbstractDiscovery::AbstractDiscovery(ThingManager& thingManager, QObject *parent)
    : QObject{parent},
      _thingManager(thingManager) {
}

void AbstractDiscovery::onThingDiscovered(const ThingInfo& info) {
    LOG_S(INFO) << "discovered thing: " << info.id();
    auto thing = ThingFactory::from(info);
    if (thing) {
        _thingManager.addThing(std::move(thing));
    }
}
