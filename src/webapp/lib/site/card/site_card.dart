import 'dart:math' as math;

import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/site/card/site_excess_controller.dart';
import 'package:iotic/site/card/data/site_service.dart';
import 'package:iotic/site/card/site_card_controller.dart';
import 'package:iotic/common/iotic_theme.dart';

import 'site_readouts.dart';

class SiteCard extends StatelessWidget {
  SiteCard({super.key});

  final SiteCardController control2 = Get.put(SiteCardController());
  final _excessControl = Get.put(SiteExcessController());
  final SiteService control = Get.find<SiteService>();

  //late final _pvLineBarsData = _line(control.pvPoints, IoticTheme.green);
  //late final _gridLineBarsData = _line(control.gridPoints, IoticTheme.orange);
  //late final _siteLineBarsData = _line(control.sitePoints, IoticTheme.yellow);

  @override
  Widget build(BuildContext context) {
    return Stack(children: [
      Card(
          clipBehavior: Clip.hardEdge,
          child: Stack(children: [
            GetBuilder<SiteCardController>(
                builder: (_) => control.pvPoints.length > 1
                    ? GestureDetector(
                        onHorizontalDragUpdate: (details) {
                          control2.drag(-0.5 * details.delta.dx);
                        },
                        child: LineChart(
                            LineChartData(
                              lineTouchData:
                                  const LineTouchData(enabled: false),
                              clipData: const FlClipData.horizontal(),
                              gridData: const FlGridData(show: false),
                              titlesData: const FlTitlesData(
                                show: false,
                              ),
                              borderData: FlBorderData(show: false),
                              minY: 15.0,
                              maxY: control2.maxY(),
                              minX: control2.minX,
                              maxX: control2.maxX,
                              lineBarsData: [
                                _line(control.pvPoints,
                                    Theme.of(context).colorScheme.secondary),
                                _line(control.sitePoints,
                                    Theme.of(context).colorScheme.primary),
                                _line(control.gridPoints,
                                    Theme.of(context).colorScheme.tertiary)
                              ],
                            ),
                            duration: control2.isFollowing
                                ? const Duration(milliseconds: 300)
                                : Duration.zero))
                    : Container()),
          ])),
      GetBuilder<SiteCardController>(
          builder: (_) => Container(
              margin: const EdgeInsets.all(4.0),
              child: IgnorePointer(
                  child: _excessControl.isVisible.value
                      ? Obx(() => LineChart(
                          LineChartData(
                            lineTouchData: const LineTouchData(enabled: false),
                            gridData: const FlGridData(show: false),
                            titlesData: const FlTitlesData(
                              show: false,
                            ),
                            borderData: FlBorderData(show: false),
                            minY: 15.0,
                            maxY: _.maxY(),
                            minX: 0,
                            maxX: _excessControl.maxX.value,
                            lineBarsData: [_dots()],
                          ),
                          duration: _excessControl.isInitialized()
                              ? const Duration(milliseconds: 1000)
                              : Duration.zero))
                      : null))),
      const Align(alignment: Alignment.topRight, child: SiteReadouts())
    ]);
  }

  // Since spots are final, we have to recreate LineChartBarData on each change.
  LineChartBarData _line(List<FlSpot> points, Color color) {
    return LineChartBarData(
      spots: points,
      dotData: const FlDotData(show: false),
      color: color,
      barWidth: 2.5,
      isCurved: false,
      //isStepLineChart: true,
      preventCurveOverShooting: true,
      shadow: const Shadow(
        offset: Offset(IoticTheme.shadowOffset, IoticTheme.shadowOffset),
        blurRadius: 0,
      ),
      belowBarData: BarAreaData(
          show: true,
          //color: color.withOpacity(0.38),
          gradient: LinearGradient(
            begin: Alignment.topCenter,
            end: Alignment.bottomCenter,
            colors: [
              //color.withOpacity(0.38),
              //color.withOpacity(0.19),
              color.withOpacity(0.4),
              color.withOpacity(0.2),
            ],
            stops: const [0.0, 0.95],
          )),
    );
  }

  LineChartBarData _dots() {
    return LineChartBarData(
      spots: [
        _excessControl.longTermExcess,
        _excessControl.longTermExcess,
        _excessControl.shortTermExcess,
      ],
      barWidth: 0,
      dotData: FlDotData(show: true, getDotPainter: _dotPainter),
      color: IoticTheme.green,
      shadow: const Shadow(
        offset: Offset(IoticTheme.shadowOffset, IoticTheme.shadowOffset),
        blurRadius: 0,
      ),
    );
  }

  FlDotPainter _dotPainter(
      FlSpot spot, double xPercentage, LineChartBarData bar, int index) {
    if (spot.y < 100.0) {
      return FlDotCirclePainter(
          radius: 0.0,
          color: Colors.black,
          strokeWidth: 0.0,
          strokeColor: Colors.black);
    }

    switch (index) {
      case 0:
        return FlDotCirclePainter(
          radius: 6.0,
          color: Colors.black,
          strokeWidth: 1.0,
          strokeColor: Colors.black,
        );
      case 1:
        return FlDotCirclePainter(
          radius: 4.0,
          color: Colors.black,
          strokeWidth: 1.0,
          strokeColor: bar.color!,
        );
      case 2:
        return FlDotCirclePainter(
          radius: 2.0,
          color: bar.color!,
          strokeWidth: 2.0,
          strokeColor: Colors.black,
        );
    }
    return FlDotCirclePainter(
      radius: 0.0,
      color: Colors.black,
      strokeWidth: 0.0,
      strokeColor: Colors.black,
    );
  }

  double maxY() {
    return math.max(
        math.max(
                control.pvPoints
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y,
                control.sitePoints
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y) +
            10,
        math.max(
                control.pvPoints
                    .skip(math.max(control.pvPoints.length - 15, 0))
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y,
                control.sitePoints
                    .skip(math.max(control.sitePoints.length - 15, 0))
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y) *
            1.05);
  }
}
