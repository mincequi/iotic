#pragma once

#include <things/sunspec/SunSpecDiscovery.h>

/**
 * This will discover further Fronius SunSpec things based on a previously
 * discovered SunSpec thing.
 *
 * A Fronius solar inverter acts as a modbus/sunspec gateway and can house
 * multiple sunspec things like smart meters and further solar inverters.
 * However, the SunSpecDiscovery stops polling for valid SunSpec things after
 * first valid unit per host.
 * This class will check for further SunSpec things on a previously discovered
 * SunSpec host.
 */
class FroniusDiscovery : public ThingsDiscovery {
public:
    FroniusDiscovery(SunSpecDiscoveryPtr sunSpecDiscovery);

private:
    void onSolarInverterDiscovered(SunSpecThingPtr thing);
    void onSmartMeterDiscovered(SunSpecThingPtr thing);

    SunSpecDiscoveryPtr _sunSpecDiscovery;
};
