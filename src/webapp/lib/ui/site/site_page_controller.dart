import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/data/thing_live_data.dart';

import '../../data/repository.dart';

class SitePageController extends GetxController {
  var thingIds = <String>{};
  final thingCount = 0.obs;

  @override
  void onReady() {
    _repo.things.listen((things) {
      var things_ = <String, ThingLiveData>{};
      for (MapEntry<String, ThingLiveData> t in things.entries) {
        if (t.value.properties.containsKey(ReadableThingProperty.isOnSite)) {
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

  final Repository _repo = Get.find<Repository>();
}
