import 'package:get/get.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';

import '../../data/repository.dart';

class ThingsController extends GetxController {
  final Repository repo = Get.find<Repository>();

  //final things = repo.things.reactive.obs;

  @override
  void onReady() {
    repo.things.listen((p0) {});
    super.onReady();
  }

  @override
  void onClose() {
    repo.things.close();
    super.onClose();
  }
}
