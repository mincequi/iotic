import 'package:iotic/things/domain/thing_property.dart';

class ThingProperties {
  final properties = <ThingProperty, dynamic>{};

  ThingProperties();

  ThingProperties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ThingProperty p = ThingProperty.values
          .firstWhere((e) => e.toString() == 'ThingProperty.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
