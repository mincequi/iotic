import 'package:flutter/material.dart';
import 'package:iotic/ui/iotic_theme.dart';

// https://medium.com/flutter-community/flutter-sliders-demystified-4b3ea65879c
class ThingSliderThumb extends SliderComponentShape {
  final double thumbRadius;
  final List<String> offsets;
  final Color color;

  const ThingSliderThumb(
      {required this.thumbRadius, required this.offsets, required this.color});

  @override
  Size getPreferredSize(bool isEnabled, bool isDiscrete) {
    return Size.fromRadius(thumbRadius);
  }

  @override
  void paint(
    PaintingContext context,
    Offset center, {
    required Animation<double> activationAnimation,
    required Animation<double> enableAnimation,
    required bool isDiscrete,
    required TextPainter labelPainter,
    required RenderBox parentBox,
    required SliderThemeData sliderTheme,
    required TextDirection textDirection,
    required double value,
    required double textScaleFactor,
    required Size sizeWithOverflow,
  }) {
    final Canvas canvas = context.canvas;

    final paint = Paint()
      //..color = color //Thumb Background Color
      ..color = sliderTheme.thumbColor!
      //..strokeWidth = 2.0
      ..style = PaintingStyle.fill;

    TextSpan span = TextSpan(
      style: TextStyle(
        fontSize: 12,
        fontWeight: FontWeight.bold,
        color: color,
      ),
      text: getValue(value),
    );

    TextPainter tp = TextPainter(
        text: span,
        textAlign: TextAlign.center,
        textDirection: TextDirection.ltr);
    tp.layout();
    Offset textCenter =
        Offset(center.dx - (tp.width / 2), center.dy - (tp.height / 2));

    final _rect = RRect.fromRectAndRadius(
      Rect.fromCenter(center: center, width: 48, height: thumbRadius * 2.0),
      Radius.circular(IoticTheme.rectangleRadius),
    );
    final _shadow = RRect.fromRectAndRadius(
      Rect.fromCenter(
          center: Offset(center.dx + IoticTheme.shadowOffset,
              center.dy + IoticTheme.shadowOffset),
          width: 48,
          height: thumbRadius * 2.0),
      Radius.circular(IoticTheme.rectangleRadius),
    );
    canvas.drawRRect(_shadow, Paint());
    canvas.drawRRect(_rect, paint);
    tp.paint(canvas, textCenter);
  }

  String getValue(double value) {
    return offsets[((offsets.length - 1) * value).round()];
  }
}
