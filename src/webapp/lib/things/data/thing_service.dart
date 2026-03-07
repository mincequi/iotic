import 'package:get/get.dart';
import 'package:iotic/logs/log_service.dart';
import 'package:iotic/things/data/thing.dart';
import 'package:iotic/things/data/thing_properties.dart';
import 'package:iotic/things/data/thing_property.dart';

class ThingService extends GetxService {
  final things = <String, ThingProperties>{}.obs;

  final _things = <String, Thing>{}.obs;
  RxList<Thing> get thingsList => RxList<Thing>.of(_things.values);

  final LogService logger = Get.find();

  ThingService() {
    things['site'] = ThingProperties(
      properties: {
        ThingPropertyKey.name: 'Site',
        ThingPropertyKey.power: 0,
      },
    );
  }
}
