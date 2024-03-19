import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/site/application/site_service.dart';

class SiteCardController extends GetxController {
  final SiteService service = Get.find<SiteService>();

  var minX = 0.0;
  var maxX = 0.0;
  var isFollowing = true;

  void drag(double dx) {
    minX += dx;
    maxX += dx;
    // If we exceed the end of our data points
    if (maxX >= service.pvPoints.last.x) {
      isFollowing = true;
    } else {
      isFollowing = false;
    }

    if (minX < service.pvPoints.first.x) {
      service.requestSiteDataHistoric(
          minX.toInt() - 120, service.pvPoints.first.x.toInt());
    }
    update();
  }

  @override
  void onInit() {
    service.update.listen((p0) {
      update();
    });
    super.onInit();
  }

  @override
  void update([List<Object>? ids, bool condition = true]) {
    if (isFollowing) {
      minX = service.pvPoints.last.x - 120;
      maxX = service.pvPoints.last.x;
    }

    super.update(ids, condition);
  }
}
