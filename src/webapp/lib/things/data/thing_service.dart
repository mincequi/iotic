import 'package:get/get.dart';
import 'package:iotic/logs/log_service.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/common/web_socket_handler.dart';
import 'package:iotic/things/data/thing.dart';
import 'package:iotic/things/data/thing_property.dart';

class ThingService extends GetxService implements WebSocketHandler {
  final WebSocketDataSource _wsDataSource;

  //final things = <String, String?>{}.obs;
  final things = <String, Thing>{}.obs;
  RxList<Thing> get thingsList => RxList<Thing>.of(things.values);

  final LogService logger = Get.find();

  ThingService(this._wsDataSource) {
    _wsDataSource.registerHandler(this);
  }

  @override
  void onInit() {
    ever(things, (_) {
      thingsList.assignAll(things.values);
    });
    super.onInit();
  }

  @override
  bool onMessage(String id, Map<String, dynamic> message) {
    _parseThing(id, message);
    return false;
  }

  void _parseThing(String id, Map<String, dynamic> properties) {
    if (!things.containsKey(id)) {
      if (!properties.containsKey('type')) {
        properties['type'] = 'unknown';
      }
      things[id] = Thing.fromMap(id, properties);
      return;
    }

    properties.forEach((key, value) {
      ThingPropertyKey p = ThingPropertyKey.values
          .firstWhere((e) => e.toString() == 'ThingPropertyKey.' + key);
      things[id]?.properties[p] = value;
    });
    // TODO: is this needed?
    things.refresh();
  }
}
