import 'package:flutter/material.dart';
import 'package:gauge_indicator/gauge_indicator.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/common/text_util.dart';

class ThingGauge extends StatelessWidget {
  const ThingGauge(this.value, this._color, {super.key});

  final double value;
  final Color _color;

  @override
  Widget build(BuildContext context) {
    return Stack(alignment: Alignment.bottomCenter, children: [
      SizedBox(
          width: 64,
          height: 64,
          child: AnimatedRadialGauge(
              duration: const Duration(seconds: 1),
              value: value,
              axis: GaugeAxis(
                  pointer: null,
                  min: 0,
                  max: 100,
                  degrees: 135,

                  /// Set the background color and axis thickness.
                  style: GaugeAxisStyle(
                    thickness: 10,
                    background: _color.withOpacity(0.4),
                    segmentSpacing: 0,
                    cornerRadius:
                        const Radius.circular(IoticTheme.rectangleRadius),
                  ),
                  progressBar: null,

                  /// Define axis segments (optional).
                  segments: [
                    GaugeSegment(
                      from: 0,
                      to: value,
                      color: _color,
                      cornerRadius: const Radius.circular(
                          IoticTheme.rectangleRadius - 1.0),
                    )
                  ]))),
      Padding(
          padding: const EdgeInsets.only(bottom: 2),
          child: Text(
            value.toStringAsFixed(0) + '\u2009%',
            style: Theme.of(context).textTheme.bodyMedium?.copyWith(
                color: value == 0 ? _color.withOpacity(0.4) : _color,
                shadows: shadows(),
                fontWeight: FontWeight.bold,
                fontSize: 12,
                textBaseline: TextBaseline.alphabetic),
          )),
    ]);
  }
}
