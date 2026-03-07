import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/things/data/thing_service.dart';

class ThingsPageController extends GetxController {
  final thingCount = 0.obs;
  var thingIds = <String>{};

  @override
  void onReady() {
    _repo.things.listen((things) {
      thingCount.value = things.length;
      thingIds = things.keys.toSet();
    });
    super.onReady();
  }

  @override
  void onClose() {
    _repo.things.close();
    super.onClose();
  }

  final ThingService _repo = Get.find<ThingService>();
}
