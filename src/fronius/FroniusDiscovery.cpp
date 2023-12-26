#include "FroniusDiscovery.h"

#include <common/Logger.h>

using namespace sunspec;

FroniusDiscovery::FroniusDiscovery(SunSpecDiscoveryPtr sunSpecDiscovery) :
    _sunSpecDiscovery(sunSpecDiscovery) {
    _sunSpecDiscovery->thingDiscovered().subscribe([this](ThingPtr thing) {
        auto sunSpecThing = std::static_pointer_cast<SunSpecThing>(thing);

        if ((sunSpecThing->unitId() >= 1 && sunSpecThing->unitId() <= 100 &&
             sunSpecThing->type() == Thing::Type::SolarInverter)) {
            onSolarInverterDiscovered(sunSpecThing);
        } else if ((sunSpecThing->unitId() >= 240 && sunSpecThing->unitId() <= 244 &&
                    sunSpecThing->type() == Thing::Type::SmartMeter)) {
            onSmartMeterDiscovered(sunSpecThing);
        }
    });
}

void FroniusDiscovery::onSolarInverterDiscovered(SunSpecThingPtr thing) {
    LOG_S(INFO) << "solar inverter found> " << "id: " << thing->sunSpecId() <<
                   ", modbusUnitId: " << thing->unitId();
}

void FroniusDiscovery::onSmartMeterDiscovered(SunSpecThingPtr thing) {
    LOG_S(INFO) << "smart meter found> " << "id: " << thing->sunSpecId() <<
                   ", modbusUnitId: " << thing->unitId();
}
