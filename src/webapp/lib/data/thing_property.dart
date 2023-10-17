// ignore_for_file: constant_identifier_names

// Dart does not allow enum inheritance. However, every WritableThingProperty
// shall also have corresponding value in ReadableThingProperty.
enum ReadableThingProperty {
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

  status,
  events
}

enum WritableThingProperty { name, pinned, offset, power_control }
