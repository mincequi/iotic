import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/data/repository.dart';

class SiteReadoutsController extends GetxController {
  final Repository repo = Get.put(Repository());

  final pvPower = 0.obs;
  final sitePower = 0.obs;
  final gridPower = 0.obs;

  @override
  void onReady() {
    repo.siteLiveData.listen((p0) {
      //int r = Random().nextInt(200) + 900;
      pvPower.value = p0.pvPower;
      sitePower.value = p0.sitePower;
      gridPower.value = p0.gridPower;
    });
    super.onReady();
  }

  @override
  void onClose() {
    repo.siteLiveData.close();
    super.onClose();
  }
}
