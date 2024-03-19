// ignore_for_file: constant_identifier_names

enum ThingProperty {
  //
  timestamp,
  pv_power,
  grid_power,

  // custom properties - things
  type,
  name,
  icon,
  pinned,

  temperature,
  power,
  actuation,
  power_control,
  // custom properties - strategies
  offset,
  thing_interval,
  time_constant,

  status,
  events
}

extension ThingIdFormatting on ThingProperty {
  String toUiString() {
    return name.replaceAll("_", " ");
  }
}
