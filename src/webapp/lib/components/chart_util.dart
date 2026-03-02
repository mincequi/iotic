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
