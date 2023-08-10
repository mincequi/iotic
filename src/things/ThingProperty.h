#pragma once

// Values taken from https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html
enum ReadableThingProperty {
    temperature = 3303,
    power = 3305
};

enum WriteableThingProperty {
    analogOutput = 3,
};
