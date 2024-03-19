import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/things/domain/thing_property.dart';

class RxProperties {
  final properties = RxMap<ThingProperty, dynamic>();

  RxProperties();

  RxProperties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ThingProperty p = ThingProperty.values
          .firstWhere((e) => e.toString() == 'ThingProperty.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
