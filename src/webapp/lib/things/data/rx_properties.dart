import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/things/data/thing_property.dart';

class RxProperties {
  final properties = RxMap<ThingPropertyKey, dynamic>();

  RxProperties();

  RxProperties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ThingPropertyKey p = ThingPropertyKey.values
          .firstWhere((e) => e.toString() == 'ThingPropertyKey.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
