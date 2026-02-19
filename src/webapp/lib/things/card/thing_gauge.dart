import 'package:flutter/material.dart';
import 'package:get/state_manager.dart';
import 'package:iotic/common/text_util.dart';

import 'dart:math' as Math;

class RadialGaugeBar {
  final RxInt value;
  final Color color;
  final String unit;
  final double thickness = 2;

  RadialGaugeBar({
    required this.value,
    required this.color,
    this.unit = '',
  });
}

class ThingGauge extends StatelessWidget {
  final List<RadialGaugeBar> bars;
  final int min;
  final int max;
  final double startAngle;
  final double endAngle;
  final double spaceBetweenBars;
  final double margins;

  const ThingGauge({
    super.key,
    required this.bars,
    required this.min,
    required this.max,
    required this.startAngle,
    required this.endAngle,
    this.spaceBetweenBars = 1,
    this.margins = 4,
  });

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
      builder: (context, constraints) {
        final size = constraints.maxHeight;

        return SizedBox(
          height: size,
          width: size + 9,
          child: Stack(
            alignment: Alignment.centerRight,
            children: [
              Padding(
                  padding: const EdgeInsets.only(right: 9.0),
                  child: CustomPaint(
                    size: Size.square(size),
                    painter: _MultiRadialGaugePainter(
                      bars: bars,
                      min: min,
                      max: max,
                      startAngle: startAngle,
                      endAngle: endAngle,
                      spaceBetweenBars: spaceBetweenBars,
                      margins: margins,
                    ),
                  )),
              Padding(
                  padding: const EdgeInsets.only(right: 4.0),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    crossAxisAlignment: CrossAxisAlignment.end,
                    mainAxisSize: MainAxisSize.max,
                    children: [
                      const SizedBox(height: 16),
                      Obx(() => Text(
                          formatWithK(
                              bars.first.value.value, bars.first.unit, 5),
                          style: textStyle(bars.first.value.value > 2
                                  ? bars.first.color
                                  : bars.first.color.withOpacity(0.4))
                              .copyWith(
                                  fontSize: 10,
                                  height: 0.95,
                                  letterSpacing: 0))),
                      Obx(() => Text(
                          formatWithK(bars.last.value.value, bars.last.unit, 5),
                          style: textStyle(bars.last.value > 2
                                  ? bars.last.color
                                  : bars.last.color.withOpacity(0.4))
                              .copyWith(
                                  fontSize: 10,
                                  height: 0.95,
                                  letterSpacing: 0))),
                    ],
                  )),
            ],
          ),
        );
      },
    );
  }
}

class _MultiRadialGaugePainter extends CustomPainter {
  final List<RadialGaugeBar> bars;
  final int min;
  final int max;
  final double startAngle;
  final double endAngle;
  final double spaceBetweenBars;
  final double margins;

  _MultiRadialGaugePainter({
    required this.bars,
    required this.min,
    required this.max,
    required this.startAngle,
    required this.endAngle,
    required this.spaceBetweenBars,
    required this.margins,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = size.center(Offset.zero);
    final startRad = _degToRad(startAngle);
    final totalSweepRad = _degToRad(endAngle - startAngle);

    double currentRadius = Math.min(size.width, size.height) / 2;

    for (final bar in bars) {
      final progress =
          ((roundToMultiple(bar.value.value, 5) - min) / (max - min))
              .clamp(0.0, 1.0);
      final sweep = totalSweepRad * progress;

      final strokeWidth = bar.thickness;
      currentRadius -= strokeWidth / 2;

      final rect = Rect.fromCircle(
        center: center,
        radius: currentRadius - margins,
      );

      final backgroundPaint = Paint()
        ..color = bar.color.withOpacity(0.4)
        ..style = PaintingStyle.stroke
        ..strokeWidth = strokeWidth
        ..strokeCap = StrokeCap.square;

      final valuePaint = Paint()
        ..color = bar.color
        ..style = PaintingStyle.stroke
        ..strokeWidth = strokeWidth
        ..strokeCap = StrokeCap.square;

      // Background arc
      canvas.drawArc(
        rect,
        Math.min(
            startRad + sweep /*+ 0.35*/, startRad + totalSweepRad /*- 0.35*/),
        totalSweepRad - sweep /*- 0.35*/,
        false,
        backgroundPaint,
      );

      // Value arc
      canvas.drawArc(
        rect,
        startRad,
        sweep,
        false,
        valuePaint,
      );

      currentRadius -= strokeWidth / 2 + spaceBetweenBars;
    }
  }

  double _degToRad(double deg) => deg * Math.pi / 180;

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => true;
}

int roundToMultiple(int value, int multiple) {
  return (value.toDouble() / multiple.toDouble()).round() * multiple;
}

String formatWithK(int value, String unit, int multiple) {
  value = roundToMultiple(value, multiple);
  if (value >= 10000) {
    double divided = value / 1000.0;
    return "${divided.toStringAsFixed(0)} k$unit";
  }
  if (value >= 1000) {
    double divided = value / 1000.0;
    return "${divided.toStringAsFixed(1)} k$unit";
  }
  return "$value $unit";
}
