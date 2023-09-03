import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
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

  final Color pvColor = Colors.lightGreen;
  final Color siteColor = Colors.yellow;
  Color gridColor() {
    if (gridPoints.isEmpty) return Colors.grey;
    return gridPoints.last.y < 0 ? Colors.teal : Colors.orange;
  }

  @override
  void onReady() {
    repo.siteLiveData.listen((p0) {
      while (pvPoints.length > limitCount) {
        pvPoints.removeAt(0);
        sitePoints.removeAt(0);
        gridPoints.removeAt(0);
      }
      var ts = timeStamp();
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
