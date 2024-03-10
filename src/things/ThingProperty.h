#pragma once

#include <magic_enum.hpp>

// Values taken from https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html
enum class Property {
    // custom properties - generic
    timestamp = 3211,
    // custom properties - site
    pv_power = 3212,
    grid_power = 3213,
    //site_power = 3214,
    // custom properties - things
    //on = 3218,
    //off = 3219,
    name = 3220,
    pinned = 3221,
    status = 3222,
    events = 3223,
    // custom properties - strategies
    offset = 3226,
    thing_interval = 3227,
    time_constant = 3228,
    // ipso properties
    generic_sensor = 3300,
    temperature = 3303,
    power = 3305,
    actuation = 3306,
    power_control = 3312,
    // ipso properties - not meant for export
    voltage = 3316, // Only object, no reusable object
    current = 3317, // Only object, no reusable object

    // Our custom static properties
    //type = 3396,
    //icon = 3398,
    //type = 5750,

    max_value = 3320
};
// We have to inject a custom range for this enum, since magic_enum only allows -128 to 128 per default.
template<>
struct magic_enum::customize::enum_range<Property> {
    static constexpr int min = (int)Property::timestamp;
    static constexpr int max = (int)Property::max_value;
    // (max - min) must be less than UINT16_MAX.
};

// Properties that are mutable from outside of a thing: e.g. config or strategy
enum class MutableProperty {
    // custom properties - things
    name = 3220,
    pinned = 3221,
    // custom properties - strategies
    offset = 3226,
    thing_interval = 3227,
    time_constant = 3228,

    // ipso properties
    actuation = 3306,
    power_control = 3312,
    // ipso properties - not meant for export
    current = 3317,
};
template<>
struct magic_enum::customize::enum_range<MutableProperty> {
    static constexpr int min = (int)MutableProperty::name;
    static constexpr int max = (int)MutableProperty::current;
    // (max - min) must be less than UINT16_MAX.
};
