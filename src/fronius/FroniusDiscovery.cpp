#include "FroniusDiscovery.h"

#include <common/Logger.h>

using namespace sunspec;

FroniusDiscovery::FroniusDiscovery(SunSpecDiscoveryPtr sunSpecDiscovery) :
    _sunSpecDiscovery(sunSpecDiscovery) {
    _sunSpecDiscovery->thingDiscovered().subscribe([](ThingPtr thing) {
        auto sunSpecThing = std::static_pointer_cast<SunSpecThing>(thing);

        if ((sunSpecThing->modbusUnitId() >= 1 &&
             sunSpecThing->modbusUnitId() <= 100 &&
             sunSpecThing->type() == Thing::Type::SolarInverter) ||
                (sunSpecThing->modbusUnitId() >= 240 &&
                 sunSpecThing->modbusUnitId() <= 244 &&
                 sunSpecThing->type() == Thing::Type::SmartMeter)) {
            LOG_S(INFO) << "potential thing found> " << "id: " << sunSpecThing->sunSpecId() <<
                           ", modbusUnitId: " << sunSpecThing->modbusUnitId();
        }
    });
}
