import 'package:iotic/data/thing_property.dart';

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
