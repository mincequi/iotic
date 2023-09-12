// ignore_for_file: constant_identifier_names

// Dart does not allow enum inheritance. However, every WritableThingProperty
// shall also have corresponding value in ReadableThingProperty.
enum ReadableThingProperty {
  type,
  name,
  icon,
  is_on_site,

  temperature,
  power,
  power_control,
  offset
}

enum WritableThingProperty { name, is_on_site, power_control, is_visible }
