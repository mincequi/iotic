#pragma once

#include <magic_enum.hpp>

// Values taken from https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html
enum class ReadableThingProperty {
    genericSensor = 3300,
    temperature = 3303,
    power = 3305,
    actuation = 3306,
    powerControl = 3312,
    multiStateSelector = 3348
};
// We have to inject a custom range for this enum, since magic_enum only allows -128 to 128 per default.
template<>
struct magic_enum::customize::enum_range<ReadableThingProperty> {
    static constexpr int min = (int)ReadableThingProperty::genericSensor;
    static constexpr int max = (int)ReadableThingProperty::multiStateSelector;
    // (max - min) must be less than UINT16_MAX.
};

enum class WriteableThingProperty {
    actuation = 3306,
    powerControl = 3312,
    dimmer = 3343
};
template<>
struct magic_enum::customize::enum_range<WriteableThingProperty> {
    static constexpr int min = (int)WriteableThingProperty::actuation;
    static constexpr int max = (int)WriteableThingProperty::dimmer;
    // (max - min) must be less than UINT16_MAX.
};
