import 'package:get/get.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/things/data/thing_type.dart';

class Thing {
  final String id;
  //final ThingType type;
  final name = RxString('');
  final properties = <ThingPropertyKey, dynamic>{}.obs;

  Thing(
    this.id,
    /*this.type*/
  );

  // Constructor from map
  Thing.fromMap(String id, Map<String, dynamic> json) : this.id = id
  /*type = ThingType.values
            .firstWhere((e) => e.toString() == 'ThingType.' + json['type'])*/
  {
    name.value = id;

    for (var kv in json.entries) {
      if (kv.key == 'type') continue;

      ThingPropertyKey p = ThingPropertyKey.values
          .firstWhere((e) => e.toString() == 'ThingPropertyKey.' + kv.key);
      properties[p] = kv.value;

      if (p == ThingPropertyKey.name) {
        name.value = kv.value;
      }
    }
  }
}
