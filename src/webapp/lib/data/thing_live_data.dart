// ignore_for_file: constant_identifier_names

enum ReadableThingProperty {
  type,
  name,
  icon,
  is_on_site,

  temperature,
  power,
  power_control
}

class ThingLiveData {
  final properties = <ReadableThingProperty, dynamic>{};

  ThingLiveData();

  ThingLiveData.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ReadableThingProperty p = ReadableThingProperty.values
          .firstWhere((e) => e.toString() == 'ReadableThingProperty.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
