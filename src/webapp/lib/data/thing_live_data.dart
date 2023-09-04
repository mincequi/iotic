enum ReadableThingProperty {
  type,
  name,
  icon,
  isOnSite,

  temperature,
  power,
  powerControl
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
