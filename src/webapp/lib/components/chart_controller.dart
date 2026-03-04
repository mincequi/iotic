import 'dart:ui' show Color;

import 'package:fl_chart/fl_chart.dart';
import 'package:get/get.dart';
import 'package:iotic/common/http_service.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/components/chart_util.dart';

class ChartController extends GetxController {
  final chartData = Rx<LineChartData>(LineChartData());
  final isLoading = RxBool(false);

  int _requestCounter = 0;
  final HttpService _httpApi = HttpService();

  /// Load chart data from the API for the given thing & property
  Future<void> loadData({
    required String thingId,
    required String propertyId,
    required DateTime date,
  }) async {
    final currentRequest = ++_requestCounter; // token for cancellation

    try {
      isLoading.value = true;

      // Fetch data points from API
      final List<List<FlSpot>> data = await _httpApi.fetchArchivedData(
        thingId,
        propertyId,
        date,
      );

      // Cancel if a newer request started
      if (currentRequest != _requestCounter) return;

      // Update chart with a single line
      _updatePoints(
          data, [IoticTheme.green, IoticTheme.blue, IoticTheme.orange]);
    } catch (e) {
      // Handle errors gracefully (optional: show snackbar)
      print('Error loading chart data: $e');
      _updatePoints([], []); // Clear chart on error
    } finally {
      if (currentRequest == _requestCounter) {
        isLoading.value = false;
      }
    }
    update();
  }

  void _updatePoints(List<List<FlSpot>> newPoints, List<Color> newColors) {
    final allSpots = newPoints.expand((e) => e).toList();

    final minX = allSpots.isEmpty
        ? 0
        : allSpots.map((e) => e.x).reduce((a, b) => a < b ? a : b);

    final maxX = allSpots.isEmpty
        ? 0
        : allSpots.map((e) => e.x).reduce((a, b) => a > b ? a : b);

    final maxY = allSpots.isEmpty
        ? 0
        : allSpots.map((e) => e.y).reduce((a, b) => a > b ? a : b);

    chartData.value = LineChartData(
      lineTouchData: const LineTouchData(enabled: false),
      clipData: const FlClipData.horizontal(),
      gridData: const FlGridData(show: false),
      titlesData: const FlTitlesData(
        show: false,
      ),
      borderData: FlBorderData(show: false),
      minY: 0.0,
      // begin x at start of day and end at end of day (24h range)
      minX: (minX ~/ 288) * 288 as double,
      maxX: ((maxX ~/ 288) + 1) * 288 as double,
      maxY: maxY as double,
      lineBarsData: List.generate(newPoints.length,
          (index) => lineData(newPoints[index], newColors[index])),
    );
  }
}
