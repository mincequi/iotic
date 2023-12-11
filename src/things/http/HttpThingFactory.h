#pragma once

#include <things/Thing.h>

// TODO: think about this. We might create things directly in the discoveries
class HttpThingFactory {
public:
    static ThingPtr from(const ThingInfo& ThingInfo);

private:
    HttpThingFactory();
};
