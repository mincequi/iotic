#ifndef THINGFACTORY_H
#define THINGFACTORY_H

#include <iot/AbstractThing.h>

class ThingFactory {
public:
    static AbstractThingPtr from(const ThingInfo& ThingInfo);

private:
    ThingFactory();
};

#endif // THINGFACTORY_H
