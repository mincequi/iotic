import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/data/thing_property.dart';

class RxProperties {
  final properties = RxMap<ReadableThingProperty, dynamic>();

  RxProperties();

  RxProperties.fromMap(Map<String, dynamic> json) {
    for (var kv in json.entries) {
      ReadableThingProperty p = ReadableThingProperty.values
          .firstWhere((e) => e.toString() == 'ReadableThingProperty.' + kv.key);
      properties[p] = kv.value;
    }
  }
}
