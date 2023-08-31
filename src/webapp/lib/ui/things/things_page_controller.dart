import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';

import '../../data/repository.dart';

class ThingsPageController extends GetxController {
  final thingIds = RxSet<String>();
  final thingCount = 0.obs;

  @override
  void onReady() {
    _repo.things.listen((p0) {
      thingCount.value = p0.length;
      thingIds.value = p0.keys.toSet();
    });
    super.onReady();
  }

  @override
  void onClose() {
    _repo.things.close();
    super.onClose();
  }

  final Repository _repo = Get.find<Repository>();
}
