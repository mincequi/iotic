// ignore_for_file: constant_identifier_names

// Dart does not allow enum inheritance. However, every WritableThingProperty
// shall also have corresponding value in ReadableThingProperty.
enum ReadableThingProperty {
  type,
  name,
  icon,
  pinned,

  temperature,
  power,
  power_control,
  offset
}

enum WritableThingProperty { name, pinned, offset, power_control, is_visible }
