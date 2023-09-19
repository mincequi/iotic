#pragma once

#include <magic_enum.hpp>

// Values taken from https://technical.openmobilealliance.org/OMNA/LwM2M/LwM2MRegistry.html
enum class DynamicProperty {
    generic_sensor = 3300,
    temperature = 3303,
    power = 3305,
    actuation = 3306,
    power_control = 3312,
    multi_state_selector = 3348,

    // Our custom static properties
    //type = 3396,
    //name = 3397,
    //icon = 3398,

    // Our custom properties
    pv_power = 3412,
    grid_power = 3413,
    site_power = 3414,

    name = 3420,
    pinned = 3421,

    max_value = 3427
};
// We have to inject a custom range for this enum, since magic_enum only allows -128 to 128 per default.
template<>
struct magic_enum::customize::enum_range<DynamicProperty> {
    static constexpr int min = (int)DynamicProperty::generic_sensor;
    static constexpr int max = (int)DynamicProperty::max_value;
    // (max - min) must be less than UINT16_MAX.
};

enum class MutableProperty {
    // IPSO properties
    actuation = 3306,
    power_control = 3312,
    dimmer = 3343,

    // Own properties
    name = 3420,
    pinned = 3421,

    power_offset = 3426,
    // special property for page visibility
    isVisible = 3427
};
template<>
struct magic_enum::customize::enum_range<MutableProperty> {
    static constexpr int min = (int)MutableProperty::actuation;
    static constexpr int max = (int)MutableProperty::isVisible;
    // (max - min) must be less than UINT16_MAX.
};
