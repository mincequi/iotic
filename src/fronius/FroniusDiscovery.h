#pragma once

#include <things/sunspec/SunSpecDiscovery.h>

class FroniusDiscovery : public ThingsDiscovery {
public:
    FroniusDiscovery(SunSpecDiscoveryPtr sunSpecDiscovery);

private:
    SunSpecDiscoveryPtr _sunSpecDiscovery;
};
