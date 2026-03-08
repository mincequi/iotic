import 'dart:math';

import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:iotic/common/iotic_theme.dart';

LineChartBarData lineData(List<FlSpot> points, Color color) {
  return LineChartBarData(
    spots: points,
    dotData: const FlDotData(show: false),
    color: color,
    barWidth: 2,
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

(num, int) zoomLevel(int maxY) {
  final List<(int, int)> _zoomLevels = [
    (10, 5), // v
    (12, 4), // v
    (15, 5), // v
    (20, 4),
    (25, 5), // v
    (30, 5), // v
    (40, 4), // v
    (50, 5), // v
    (60, 4), // v
    (80, 4), // v
  ];

  for (int i = 1; i <= 2; i++) {
    for (var (threshold, lines) in _zoomLevels) {
      if (maxY < threshold * pow(10, i)) {
        return (threshold * pow(10, i), lines);
      }
    }
  }
  return (maxY.toDouble(), 5);
}

List<HorizontalLine> horizontalLines((num, int) zoomLevelResult, {int factor = 1}) =>
    List.generate(zoomLevelResult.$2, (index) {
      final y = zoomLevelResult.$1 / zoomLevelResult.$2 * (index + 1);
      final yReadout = y * factor;
      return HorizontalLine(
        y: y,
        color: IoticTheme.other,
        strokeWidth: 0.2,
        label: HorizontalLineLabel(
          show: true,
          alignment: Alignment.bottomLeft,
          padding: const EdgeInsets.only(left: 3, top: 0),
          style: const TextStyle(
            color: IoticTheme.other,
            fontSize: 10,
            fontWeight: FontWeight.normal,
          ),
          labelResolver: (line) =>
              yReadout >= 1000 ? "${(yReadout / 1000).toStringAsFixed(0)} kW" : "${yReadout.toStringAsFixed(0)} W",
        ),
      );
    });
