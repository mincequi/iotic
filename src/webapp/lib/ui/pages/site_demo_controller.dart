import 'dart:async';
import 'dart:math' as math;
import 'package:fl_chart/fl_chart.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';

class SiteDemoController extends GetxController {
  late Timer _timer;

  final limitCount = 60;
  double xValue = 0;
  double step = 0.2;

  final pvPoints = List.filled(1, const FlSpot(0.0, 0.0), growable: true).obs;
  final consumptionPoints =
      List.filled(1, const FlSpot(0.0, 0.0), growable: true).obs;

  @override
  void onReady() {
    _timer = Timer.periodic(const Duration(milliseconds: 100), (timer) {
      while (pvPoints.length > limitCount) {
        pvPoints.removeAt(0);
        consumptionPoints.removeAt(0);
      }
      pvPoints.add(FlSpot(xValue, 2 * (1.0 + math.sin(xValue / 5))));
      consumptionPoints.add(FlSpot(xValue, 1.0 + math.cos(xValue / 5)));
      xValue += step;
    });
    super.onReady();
  }

  @override
  void onClose() {
    _timer.cancel();
    super.onClose();
  }
}
