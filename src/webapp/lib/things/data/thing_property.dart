// ignore_for_file: constant_identifier_names

enum ThingPropertyKey {
  //
  timestamp,
  pv_power,
  grid_power,
  site_power, // deprecated
  short_term_grid_power,
  long_term_grid_power,

  // custom properties - things
  type,
  name,
  icon,
  pinned,

  temperature,
  power,
  actuation,
  power_control,

  phases,
  next_phases,

  // custom properties - strategies
  offset,
  thing_interval,
  time_constant,
  countdown,

  status,
  events
}

extension ThingIdFormatting on ThingPropertyKey {
  String toUiString() {
    return name.replaceAll("_", " ");
  }
}
