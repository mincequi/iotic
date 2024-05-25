import 'dart:math' as math;

import 'package:collection/collection.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/site/card/data/site_service.dart';

class SiteCardController extends GetxController {
  final SiteService service = Get.find<SiteService>();

  var minX = 0.0;
  var maxX = 0.0;
  var isFollowing = true;

  double maxY() {
    final i = lowerBound(service.pvPoints.map((e) => e.x).toList(), minX,
        compare: (double s, double t) {
      if (s < t)
        return -1;
      else if (s > t)
        return 1;
      else
        return 0;
    });
    final j = lowerBound(service.pvPoints.map((e) => e.x).toList(), maxX,
        compare: (double s, double t) {
      if (s <= t)
        return -1;
      else if (s > t)
        return 1;
      else
        return 0;
    });
    return (i < j
            ? math.max(
                service.pvPoints
                    .getRange(i, j)
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y,
                service.sitePoints
                    .getRange(i, j)
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y)
            : math.max(service.pvPoints[i].x, service.sitePoints[i].x)) *
        1.05;
  }

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
    if (isFollowing && service.pvPoints.isNotEmpty) {
      minX = service.pvPoints.last.x - 120;
      maxX = service.pvPoints.last.x;
    }

    super.update(ids, condition);
  }
}
