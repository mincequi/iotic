import 'package:flutter/material.dart';

class OdometerDigit extends StatefulWidget {
  final double digit;
  final TextStyle? style;
  final Duration duration;
  final bool isLastDigit;

  const OdometerDigit({
    super.key,
    required this.digit,
    required this.style,
    this.duration = const Duration(milliseconds: 1000),
    this.isLastDigit = false,
  });

  @override
  State<OdometerDigit> createState() => _OdometerDigitState();
}

class _OdometerDigitState extends State<OdometerDigit> {
  double _currentValue = 0;

  @override
  void initState() {
    super.initState();
    _currentValue = widget.digit.toDouble();
  }

  @override
  void didUpdateWidget(covariant OdometerDigit oldWidget) {
    super.didUpdateWidget(oldWidget);

    double oldDigit = oldWidget.digit;
    double newDigit = widget.digit;

    if (newDigit == oldDigit) return;

    double diff = newDigit - oldDigit;

    // forward-only wrap (9 → 0)
    if (diff < 0) {
      diff += 10;
    }

    setState(() {
      _currentValue += diff;
    });
  }

  @override
  Widget build(BuildContext context) {
    final fontSize = widget.style?.fontSize ?? 14;
    final height = fontSize * 0.85;
    double viewportHeight = height;
    if (widget.isLastDigit) {
      viewportHeight *=
          1.4; // slightly taller for last digit to show more of the fraction
    }
    final centerOffset = (viewportHeight - height) / 2;

    return SizedBox(
      width: fontSize * 0.6,
      height: viewportHeight,
      child: ClipRect(
        child: TweenAnimationBuilder<double>(
          tween: Tween(
            begin: _currentValue,
            end: _currentValue,
          ),
          duration: widget.duration,
          //curve: Curves.easeOutCubic,
          builder: (context, value, child) {
            return Transform.translate(
              offset: Offset(0, (-(value % 10) - 10) * height + centerOffset),
              child: child,
            );
          },
          child: Column(
            children: List.generate(
              30,
              (i) => SizedBox(
                height: height,
                child: Center(
                  child: Text(
                    (i % 10).toString(),
                    style: widget.style?.copyWith(height: 0.9),
                  ),
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class Odometer extends StatelessWidget {
  final double value;
  final int digits;
  final TextStyle? style;

  const Odometer({
    super.key,
    required this.value,
    this.digits = 6,
    required this.style,
  });

  @override
  Widget build(BuildContext context) {
    final intPart = value.floor();
    final fraction = value - intPart;

    // Integer digits
    final intDigits =
        intPart.toString().split('').map((e) => double.parse(e)).toList();

    // Add leading zeros
    while (intDigits.length < digits) {
      intDigits.insert(0, 0);
    }

    // Create ONE fractional digit (0–9, fractional scroll)
    final fractionalDigit = fraction * 10;

    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        // Integer digits
        ...intDigits.map((d) => OdometerDigit(
              digit: d,
              style: style,
              isLastDigit: false,
            )),

        // Decimal point
        Text('.', style: style),

        // Fractional digit
        OdometerDigit(
          digit: fractionalDigit,
          style: style,
          isLastDigit: true,
        ),
      ],
    );
  }
}
