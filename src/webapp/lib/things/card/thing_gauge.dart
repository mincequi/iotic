import 'package:flutter/material.dart';
import 'package:iotic/common/text_util.dart';

import 'dart:math' as Math;

class RadialGaugeBar {
  final double value;
  final Color color;
  final double thickness;

  RadialGaugeBar({
    required this.value,
    required this.color,
    this.thickness = 3,
  });
}

class ThingGauge extends StatelessWidget {
  final List<RadialGaugeBar> bars;
  final double min;
  final double max;
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
          width: size,
          child: Stack(
            alignment: Alignment.centerRight,
            children: [
              CustomPaint(
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
              ),
              Padding(
                  padding: EdgeInsets.only(right: 4.0),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    crossAxisAlignment: CrossAxisAlignment.center,
                    mainAxisSize: MainAxisSize.max,
                    children: [
                      SizedBox(height: 10),
                      // small space character
                      //const Text('\u2007'),
                      Text(bars.first.value.toStringAsFixed(0) + '%',
                          style: textStyle(bars.first.value > 0
                              ? bars.first.color
                              : bars.first.color.withOpacity(0.4))),
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
  final double min;
  final double max;
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
      final progress = ((bar.value - min) / (max - min)).clamp(0.0, 1.0);
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
        Math.min(startRad + sweep + 0.35, startRad + totalSweepRad - 0.35),
        totalSweepRad - sweep - 0.35,
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
