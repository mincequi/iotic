#pragma once

#include <magic_enum.hpp>

// Values taken from https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html
enum ReadableThingProperty {
    genericSensor = 3300,
    temperature = 3303,
    power = 3305,
    multiStateSelector = 3348
};
// We have to inject a custom range for this enum, since magic_enum only allows -128 to 128 per default.
template<>
struct magic_enum::customize::enum_range<ReadableThingProperty> {
    static constexpr int min = genericSensor;
    static constexpr int max = multiStateSelector;
    // (max - min) must be less than UINT16_MAX.
};

enum WriteableThingProperty {
    analogOutput = 3,
};
