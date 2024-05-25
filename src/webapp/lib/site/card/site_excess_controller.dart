import 'dart:math';

import 'package:fl_chart/fl_chart.dart';
import 'package:get/get.dart';
import 'package:iotic/site/card/data/site_service.dart';
import 'package:iotic/things/data/thing_properties.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/common/web_socket_handler.dart';

class SiteExcessController extends GetxController implements WebSocketHandler {
  SiteExcessController() {
    Get.find<WebSocketDataSource>().registerHandler(this);
  }

  final SiteService _service = Get.find<SiteService>();

  final maxX = 0.0.obs;
  var shortTermExcess = const FlSpot(0, 0);
  var longTermExcess = const FlSpot(0, 0);
  final isVisible = false.obs;
  bool isInitialized() {
    return _updateCount > 1;
  }

  int _updateCount = 0;

  @override
  void onInit() {
    _service.update.listen((p0) {
      maxX.value = _service.pvPoints.last.x;
      //shortTermExcess = _service.pvPoints.last;
      //longTermExcess = _service.pvPoints.last;
    });
    super.onInit();
  }

  @override
  bool onMessage(String id, Map<String, dynamic> entry) {
    if (id != "site") return false;

    final props = ThingProperties.fromMap(entry);
    if (props.properties.isEmpty) return false;
    if (props.properties[ThingPropertyKey.timestamp] == null ||
        props.properties[ThingPropertyKey.short_term_grid_power] == null ||
        props.properties[ThingPropertyKey.long_term_grid_power] == null)
      return false;

    //isVisible.value =
    //    props.properties[ThingPropertyKey.short_term_grid_power].toDouble() <
    //            0 &&
    //        props.properties[ThingPropertyKey.long_term_grid_power] < 0;

    double sitePower = props.properties[ThingPropertyKey.pv_power].toDouble() +
        props.properties[ThingPropertyKey.grid_power].toDouble();

    shortTermExcess = FlSpot(
        props.properties[ThingPropertyKey.timestamp].toDouble(),
        max(
            -props.properties[ThingPropertyKey.short_term_grid_power]
                    .toDouble() +
                sitePower,
            0.0));
    longTermExcess = FlSpot(
        props.properties[ThingPropertyKey.timestamp].toDouble(),
        max(
            -props.properties[ThingPropertyKey.long_term_grid_power]
                    .toDouble() +
                sitePower,
            0.0));

    _updateCount++;

    return true;
  }
}
