import 'package:iotic/things/data/thing_property.dart';

class ThingProperties {
  final properties = <ThingPropertyKey, dynamic>{};

  // Contructor with optional properties map
  ThingProperties({Map<ThingPropertyKey, dynamic>? properties}) {
    if (properties != null) {
      this.properties.addAll(properties);
    }
  }

  ThingProperties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ThingPropertyKey p = ThingPropertyKey.values.firstWhere((e) => e.toString() == 'ThingPropertyKey.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
