import 'package:flutter/material.dart';

class OdometerDigit extends StatefulWidget {
  final double digit;
  final TextStyle? style;
  final Duration duration;

  const OdometerDigit({
    super.key,
    required this.digit,
    required this.style,
    this.duration = const Duration(milliseconds: 400),
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

    return SizedBox(
      width: fontSize * 0.6,
      height: height,
      child: ClipRect(
        child: TweenAnimationBuilder<double>(
          tween: Tween(
            begin: _currentValue,
            end: _currentValue,
          ),
          duration: widget.duration,
          curve: Curves.easeOutCubic,
          builder: (context, value, child) {
            return Transform.translate(
              offset: Offset(0, (-(value % 10) - 10) * height),
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

    final digits =
        intPart.toString().split('').map((e) => double.parse(e)).toList();

    // Extend with leading zeros if necessary
    while (digits.length < this.digits) {
      digits.insert(0, 0);
    }
    digits[digits.length - 1] += fraction;

    return Row(
      mainAxisSize: MainAxisSize.min,
      children: List.generate(digits.length, (index) {
        final digitWidget = OdometerDigit(
          digit: digits[index],
          style: style,
          //isRelevant: (index >= firstNonZero) && (index < chars.length - 1) ||
          //    ((index == chars.length - 1) && chars[index] != '0'),
          //isRelevant: true,
          //isLastDigit: index == chars.length - 1,
        );

        // Insert dot before last digit
        if (index == digits.length - 1) {
          return Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              Padding(
                padding: EdgeInsets.symmetric(
                    horizontal: (style?.fontSize ?? 14) * 0.1),
                child: Text(
                  '.',
                  style: style,
                ),
              ),
              digitWidget,
            ],
          );
        }

        return digitWidget;
      }),
    );
  }
}
