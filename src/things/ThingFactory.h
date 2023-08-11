#pragma once

#include <things/Thing.h>

// TODO: think about this. We might create things directly in the discoveries
class ThingFactory {
public:
    static ThingPtr from(const ThingInfo& ThingInfo);

private:
    static void registerFactory();
    ThingFactory();
};
