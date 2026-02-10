import 'package:collection/collection.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:get/get.dart';
import 'package:iotic/common/web_socket_data_source.dart';

class SiteService extends GetxService {
  var pvPoints = List<FlSpot>.empty(growable: true);
  var gridPoints = List<FlSpot>.empty(growable: true);
  var sitePoints = List<FlSpot>.empty(growable: true);

  final update = RxInt(0);

  final _dataSource = Get.find<WebSocketDataSource>();

  void requestSiteDataHistoric(int from, int to) {
    pvPoints.insert(0, FlSpot(from.toDouble(), 0.0));
    gridPoints.insert(0, FlSpot(from.toDouble(), 0.0));
    sitePoints.insert(0, FlSpot(from.toDouble(), 0.0));
    _dataSource.requestSiteDataHistoric(from, to);
  }

  @override
  void onReady() {
    _dataSource.siteDataHistoric.listen((p0) {
      final pv = List<FlSpot>.empty(growable: true);
      final grid = List<FlSpot>.empty(growable: true);
      final site = List<FlSpot>.empty(growable: true);

      for (var i = 0; i < p0.ts.length; ++i) {
        pv.add(FlSpot(p0.ts[i].toDouble(), p0.pvPower[i].toDouble()));
        grid.add(FlSpot(p0.ts[i].toDouble(), p0.gridPower[i].toDouble()));
        site.add(FlSpot(p0.ts[i].toDouble(),
            p0.pvPower[i].toDouble() + p0.gridPower[i].toDouble()));
      }

      add(pvPoints, pv);
      add(gridPoints, grid);
      add(sitePoints, site);

      update.value = update.value++;
    });

    _dataSource.siteDataLive.listen((p0) {
      final ts = p0.ts.toDouble();
      pvPoints.add(FlSpot(ts, p0.pvPower.toDouble()));
      gridPoints.add(FlSpot(ts, p0.gridPower.toDouble()));
      sitePoints.add(FlSpot(ts, -p0.sitePower.toDouble()));

      update.value = update.value++;
    });
    super.onReady();
  }

  @override
  void onClose() {
    _dataSource.siteDataLive.close();
    super.onClose();
  }

  void add(List<FlSpot> oldList, List<FlSpot> newList) {
    if (newList.isEmpty) return;

    final i = lowerBound(oldList, newList.first, compare: (FlSpot s, FlSpot t) {
      if (s.x < t.x)
        return -1;
      else if (s.x > t.x)
        return 1;
      else
        return 0;
    });

    final j = lowerBound(oldList, newList.last, compare: (FlSpot s, FlSpot t) {
      if (s.x <= t.x)
        return -1;
      else if (s.x > t.x)
        return 1;
      else
        return 0;
    });

    oldList.removeRange(i, j);
    oldList.insertAll(i, newList);
  }
}
