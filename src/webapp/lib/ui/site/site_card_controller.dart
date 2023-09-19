import 'package:fl_chart/fl_chart.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/data/repository.dart';

class SiteCardController extends GetxController {
  final Repository repo = Get.find<Repository>();

  static double timeStamp() => DateTime.now().millisecondsSinceEpoch / 1000;

  // We have to extend the points by 2, because there are artefacts when curve
  // moves out on the left side.
  final limitCount = 62;

  final pvPoints = List<FlSpot>.empty().obs;
  final sitePoints = List<FlSpot>.empty().obs;
  final gridPoints = List<FlSpot>.empty().obs;

  @override
  void onReady() {
    repo.siteHistoricData.listen((p0) {
      pvPoints.clear();
      sitePoints.clear();
      gridPoints.clear();
      for (var i = 0; i < p0.ts.length; ++i) {
        pvPoints.add(FlSpot(p0.ts[i].toDouble(), p0.pvPower[i].toDouble()));
        sitePoints
            .add(FlSpot(p0.ts[i].toDouble(), -p0.sitePower[i].toDouble()));
        gridPoints.add(FlSpot(p0.ts[i].toDouble(), p0.gridPower[i].toDouble()));
      }
    });

    repo.siteLiveData.listen((p0) {
      var ts = p0.ts.toDouble();
      while (pvPoints.length > limitCount ||
          (pvPoints.isNotEmpty && pvPoints[0].x < (ts - (63 * 2)))) {
        pvPoints.removeAt(0);
        sitePoints.removeAt(0);
        gridPoints.removeAt(0);
      }

      if (pvPoints.isEmpty) {
        pvPoints.add(FlSpot(ts - 4, p0.pvPower.toDouble()));
        sitePoints.add(FlSpot(ts - 4, -p0.sitePower.toDouble()));
        gridPoints.add(FlSpot(ts - 4, p0.gridPower.toDouble()));
        pvPoints.add(FlSpot(ts - 2, p0.pvPower.toDouble()));
        sitePoints.add(FlSpot(ts - 2, -p0.sitePower.toDouble()));
        gridPoints.add(FlSpot(ts - 2, p0.gridPower.toDouble()));
      }
      pvPoints.add(FlSpot(ts, p0.pvPower.toDouble()));
      sitePoints.add(FlSpot(ts, -p0.sitePower.toDouble()));
      gridPoints.add(FlSpot(ts, p0.gridPower.toDouble()));
    });
    super.onReady();
  }

  @override
  void onClose() {
    repo.siteLiveData.close();
    super.onClose();
  }
}
