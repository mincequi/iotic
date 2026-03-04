import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/components/chart_util.dart';
import 'package:iotic/site/card/data/site_service.dart';
import 'package:iotic/site/card/site_card_controller.dart';

import 'site_readouts.dart';

class SiteCard extends StatelessWidget {
  SiteCard({super.key});

  final SiteCardController control2 = Get.put(SiteCardController());
  final SiteService control = Get.find<SiteService>();

  @override
  Widget build(BuildContext context) {
    return SizedBox(
        height: 194,
        width: double.infinity,
        child: Stack(children: [
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
                                    lineData(
                                        control.pvPoints,
                                        Theme.of(context)
                                            .colorScheme
                                            .secondary),
                                    lineData(control.sitePoints,
                                        Theme.of(context).colorScheme.primary),
                                    lineData(control.gridPoints,
                                        Theme.of(context).colorScheme.tertiary)
                                  ],
                                ),
                                duration: control2.isFollowing
                                    ? const Duration(milliseconds: 300)
                                    : Duration.zero))
                        : Container()),
              ])),
          const Align(alignment: Alignment.topRight, child: SiteReadouts())
        ]));
  }
}
