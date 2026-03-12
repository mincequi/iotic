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
    (100, 2), // v
    //(1200, 4), // v
    (150, 3), // v
    (200, 2),
    //(2500, 5), // v
    (300, 3), // v
    (400, 2), // v
    //(5000, 5), // v
    (600, 3), // v
    (800, 2), // v
  ];

  // 1,33352143 / R8
  final List<(int, int)> _zoomLevelsAlt = [
    (1000, 4), // v
    (1200, 3), // v
    (1800, 3), // v
    (2400, 3), // v
    (3000, 3), // v
    (4000, 4), // v
    (6000, 3), // v
    (7500, 3), // v
  ];

  for (int i = 0; i <= 2; i++) {
    for (var (threshold, lines) in _zoomLevels) {
      if (maxY < threshold * pow(10, i)) {
        return (threshold * pow(10, i), lines);
      }
    }
  }
  return (maxY.toDouble(), 5);
}

List<HorizontalLine> horizontalLines((num, int) zoomLevelResult) => List.generate(zoomLevelResult.$2, (index) {
      final y = zoomLevelResult.$1 / zoomLevelResult.$2 * (index + 1);
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
          labelResolver: (line) {
            return y > 1500 ? "${(y / 1000).toStringAsFixed(0)} kW" : "${y.toStringAsFixed(0)} W";
          },
        ),
      );
    });
