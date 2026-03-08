import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:geolocator/geolocator.dart';
import 'package:get/get.dart';
import 'package:iotic/common/http_service.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/components/chart_util.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/things/data/thing_service.dart';
import 'package:solar_calculator/solar_calculator.dart';

class ChartController extends GetxController {
  final chartData = Rx<LineChartData>(LineChartData());
  final isLoading = RxBool(false);

  int _requestCounter = 0;
  final HttpService _httpApi = HttpService();
  final ThingService _thingService = Get.find<ThingService>();

  Future<Position> getLocation() async {
    bool serviceEnabled = await Geolocator.isLocationServiceEnabled();
    if (!serviceEnabled) {
      throw Exception("Location services disabled");
    }

    LocationPermission permission = await Geolocator.checkPermission();
    if (permission == LocationPermission.denied) {
      permission = await Geolocator.requestPermission();
    }

    return await Geolocator.getCurrentPosition();
  }

  Position? _position;
  late DateTime _lastDate;

  @override
  void onInit() {
    super.onInit();
    getLocation().then((position) {
      _position = position;
    }).catchError((error) {
      // Handle location errors here if needed
      _position = null;
    });
  } // Handle location errors here if needed

  Future<void> loadData({
    required String thingId,
    required String propertyId,
    required DateTime date,
  }) async {
    _lastDate = date;

    final currentRequest = ++_requestCounter;

    try {
      isLoading.value = true;

      List<List<FlSpot>> data;
      List<Color> colors;

      if (thingId == 'site') {
        data = await fetchSiteData(date);
        colors = [
          IoticTheme.green,
          IoticTheme.yellow,
          IoticTheme.orange,
        ];
      } else {
        data = await _httpApi.fetchArchivedData(
          thingId,
          propertyId,
          date,
        );
        colors = [
          IoticTheme.green,
          IoticTheme.blue,
          IoticTheme.orange,
        ];
      }

      if (currentRequest != _requestCounter) return;

      _updatePoints(data, colors);
    } catch (e) {
      _updatePoints([], []);
    } finally {
      if (currentRequest == _requestCounter) {
        isLoading.value = false;
      }
    }

    update();
  }

  Future<List<List<FlSpot>>> fetchSiteData(DateTime date) async {
    // Fetch power data for all solar inverters and smart meters, and combine into one dataset
    final inverters =
        _thingService.things.entries.where((entry) => entry.value.properties[ThingPropertyKey.type] == 'solarInverter');
    final smartMeters =
        _thingService.things.entries.where((entry) => entry.value.properties[ThingPropertyKey.type] == 'smartMeter');

    final invertersDataFuture =
        Future.wait(inverters.map((entry) => _httpApi.fetchArchivedData(entry.key, 'power', date)));
    final smartMetersDataFuture =
        Future.wait(smartMeters.map((entry) => _httpApi.fetchArchivedData(entry.key, 'power', date)));

    // Sum power from all inverters
    final invertersData = await invertersDataFuture;
    final List<FlSpot> pvSpots = List.filled(288, const FlSpot(0, 0), growable: false);

    for (var inverterData in invertersData) {
      for (int i = 0; i < inverterData.length; i++) {
        for (int j = 0; j < inverterData[i].length; j++) {
          pvSpots[j] = FlSpot(j.toDouble(), pvSpots[j].y + inverterData[i][j].y);
        }
      }
    }

    // Sum power from all smart meters
    final smartMetersData = await smartMetersDataFuture;
    final List<FlSpot> gridSpots = List.filled(288, const FlSpot(0, 0), growable: false);
    for (var smartMeterData in smartMetersData) {
      for (int i = 0; i < smartMeterData.length; i++) {
        for (int j = 0; j < smartMeterData[i].length; j++) {
          gridSpots[j] = FlSpot(j.toDouble(), gridSpots[j].y + smartMeterData[i][j].y);
        }
      }
    }

    final List<FlSpot> siteSpots = List.filled(288, const FlSpot(0, 0), growable: false);
    for (int i = 0; i < 288; i++) {
      siteSpots[i] = FlSpot(i.toDouble(), gridSpots[i].y + pvSpots[i].y);
    }

    return [
      pvSpots,
      siteSpots,
      gridSpots,
    ];
  }

  void _updatePoints(List<List<FlSpot>> newPoints, List<Color> newColors) {
    final allSpots = newPoints.expand((e) => e).toList();

    final minX = allSpots.isEmpty ? 0 : allSpots.map((e) => e.x).reduce((a, b) => a < b ? a : b);

    final maxX = allSpots.isEmpty ? 0 : allSpots.map((e) => e.x).reduce((a, b) => a > b ? a : b);

    final maxY = allSpots.isEmpty ? 0 : allSpots.map((e) => e.y).reduce((a, b) => a > b ? a : b);

    // compute zoom level and multiply first value by 10
    final zoomLevelResult = zoomLevel(maxY.toInt());

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
      maxY: zoomLevelResult.$1.toDouble(),
      lineBarsData: List.generate(newPoints.length, (index) => lineData(newPoints[index], newColors[index])),
      extraLinesData: ExtraLinesData(
        horizontalLines: horizontalLines(zoomLevelResult, factor: 10),
        verticalLines: _position == null
            ? []
            : [
                VerticalLine(
                  x: _noonToX(_lastDate, _position?.latitude ?? 0, _position?.longitude ?? 0),
                  color: IoticTheme.other,
                  strokeWidth: 0.2,
                  label: VerticalLineLabel(
                    show: true,
                    alignment: Alignment.bottomRight,
                    padding: const EdgeInsets.only(left: 3, bottom: 3),
                    style: const TextStyle(fontSize: 10, color: IoticTheme.other),
                    labelResolver: (line) => formatTime(line.x),
                  ),
                )
              ],
      ),
    );
  }

  double _noonToX(DateTime date, double latitude, double longitude) {
    Instant now = Instant.fromDateTime(date.toUtc());
    SolarCalculator solarCalc = SolarCalculator(now, latitude, longitude);
    double minute =
        (solarCalc.sunTransitTime.hour * 60 + solarCalc.sunTransitTime.minute + solarCalc.sunTransitTime.second / 60) /
            5;
    return minute;
  }

  String formatTime(double x) {
    int totalMinutes = (x * 5).toInt();
    // Convert from UTC to local time by adding the timezone offset
    DateTime now = DateTime.now();
    int timezoneOffsetMinutes = now.timeZoneOffset.inMinutes;
    int localTotalMinutes = (totalMinutes + timezoneOffsetMinutes) % (24 * 60);
    int hours = localTotalMinutes ~/ 60;
    int minutes = localTotalMinutes % 60;

    return '${hours.toString().padLeft(2, '0')}:${minutes.toString().padLeft(2, '0')}';
  }
}
