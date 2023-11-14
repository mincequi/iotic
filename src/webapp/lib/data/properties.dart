import 'package:iotic/data/thing_property.dart';

class Properties {
  final properties = <ThingProperty, dynamic>{};

  Properties();

  Properties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ThingProperty p = ThingProperty.values
          .firstWhere((e) => e.toString() == 'ThingProperty.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
