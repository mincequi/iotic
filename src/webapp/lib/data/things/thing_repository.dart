import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/data/logs/logging_service.dart';
import 'package:iotic/data/web_socket_data_source.dart';
import 'package:iotic/data/things/thing_properties.dart';
import 'package:iotic/data/web_socket_handler.dart';

class ThingRepository implements WebSocketHandler {
  final WebSocketDataSource _wsDataSource;

  final things = <String, ThingProperties>{}.obs;

  final LoggingService logger = Get.find();

  ThingRepository(this._wsDataSource) {
    _wsDataSource.registerHandler(this);
  }

  @override
  bool onMessage(String id, Map<String, dynamic> message) {
    return false;
  }

  void _parseThing(MapEntry<String, dynamic> entry) {
    if (!things.containsKey(entry.key)) {
      things[entry.key] = ThingProperties.fromMap(entry.value);
      return;
    }
    ThingProperties.fromMap(entry.value).properties.forEach((key, value) {
      things[entry.key]?.properties[key] = value;
    });
    // TODO: is this needed?
    things.refresh();
  }
}
