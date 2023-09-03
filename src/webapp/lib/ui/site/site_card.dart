import 'dart:math' as math;

import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';
import 'package:iotic/ui/site/site_card_controller.dart';
import 'package:iotic/ui/site/site_readouts.dart';

class SiteCard extends StatelessWidget {
  SiteCard({super.key});

  final SiteCardController c = Get.put(SiteCardController());

  @override
  Widget build(BuildContext context) {
    return Card(
        child: Stack(children: [
      //Padding(
      //  padding: const EdgeInsets.only(
      //      bottom: _cornerRadius, top: _cornerRadius),
      /*child:*/ Obx(() => c.pvPoints.length > 1
          ? LineChart(
              LineChartData(
                lineTouchData: const LineTouchData(enabled: false),
                clipData: const FlClipData.horizontal(),
                gridData: const FlGridData(show: false),
                /*gridData: FlGridData(
                      getDrawingHorizontalLine: (value) => const FlLine(
                          color: Colors.blueGrey, strokeWidth: 0.4),
                      show: true,
                      drawVerticalLine: false,
                    ),*/
                titlesData: const FlTitlesData(
                  show: false,
                ),
                borderData: FlBorderData(show: false),
                minY: 10.0,
                maxY: maxY(),
                // TOD
                minX: c.pvPoints[1].x,
                maxX: c.pvPoints.last.x,
                lineBarsData: [
                  sinLine(c.pvPoints, c.pvColor, true),
                  sinLine(c.sitePoints, c.siteColor, true),
                  sinLine(c.gridPoints, Colors.orange, true)
                ],
              ),
              duration: const Duration(milliseconds: 300),
            )
          : Container()),
      //),
      const Align(alignment: Alignment.topRight, child: SiteReadouts())
    ]));
  }

  LineChartBarData sinLine(List<FlSpot> points, Color color, bool renderBelow) {
    return LineChartBarData(
      spots: points,
      dotData: const FlDotData(show: false),
      color: color,
      /*
      gradient: LinearGradient(
        colors: [color.withOpacity(0), color],
        stops: const [0.0, 0.375],
      ),
      */
      barWidth: 2,
      isCurved: true,
      preventCurveOverShooting: true,
      shadow: const Shadow(
        blurRadius: 8,
      ),
      belowBarData: BarAreaData(
          show: renderBelow,
          //color: color.withOpacity(0.38),
          gradient: LinearGradient(
            begin: Alignment.topCenter,
            end: Alignment.bottomCenter,
            colors: [
              //color.withOpacity(0.38),
              //color.withOpacity(0.19),
              color.withOpacity(0.3),
              color.withOpacity(0.0),
            ],
            stops: const [0.0, 0.95],
          )),
    );
  }

  double maxY() {
    return math.max(
        math.max(
                c.pvPoints
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y,
                c.sitePoints
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y) +
            10,
        math.max(
                c.pvPoints
                    .skip(math.max(c.pvPoints.length - 15, 0))
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y,
                c.sitePoints
                    .skip(math.max(c.sitePoints.length - 15, 0))
                    .reduce((curr, next) => (curr.y > next.y) ? curr : next)
                    .y) *
            1.05);
  }
}
