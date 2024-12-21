import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/src/simple/get_controllers.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/things/data/thing_key.dart';

class SettingsPageController extends GetxController {
  final sections = RxList<ThingKey>();
  late final settings = _repo.settings;

  @override
  void onReady() {
    _repo.settings.listen((s0) {
      sections.assignAll(s0.keys);
    });
    super.onReady();
  }

  @override
  void onClose() {
    _repo.settings.close();
    super.onClose();
  }

  final WebSocketDataSource _repo = Get.find<WebSocketDataSource>();
}
