import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/data/things/thing_properties.dart';

import '../../data/web_socket_data_source.dart';
import '../../data/things/thing_property.dart';

class SitePageController extends GetxController {
  var thingIds = <String>{};
  final thingCount = 0.obs;

  @override
  void onReady() {
    _repo.things.listen((things) {
      var things_ = <String, ThingProperties>{};
      for (MapEntry<String, ThingProperties> t in things.entries) {
        if (t.value.properties[ThingPropertyKey.pinned] ?? false) {
          things_[t.key] = t.value;
        }
      }

      thingCount.value = things_.length;
      thingIds = things_.keys.toSet();
    });
    super.onReady();
  }

  @override
  void onClose() {
    _repo.things.close();
    super.onClose();
  }

  final WebSocketDataSource _repo = Get.find<WebSocketDataSource>();
}
