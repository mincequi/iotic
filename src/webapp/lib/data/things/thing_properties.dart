import 'package:iotic/data/things/thing_property.dart';

class ThingProperties {
  final properties = <ThingPropertyKey, dynamic>{};

  ThingProperties();

  ThingProperties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ThingPropertyKey p = ThingPropertyKey.values
          .firstWhere((e) => e.toString() == 'ThingPropertyKey.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
