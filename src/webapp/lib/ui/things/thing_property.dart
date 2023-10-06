import 'package:flutter/material.dart';

class ThingProperty extends StatelessWidget {
  const ThingProperty(this._icon, this._value, this._unit, {super.key});

  final IconData _icon;
  final double _value;
  final String _unit;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
        width: 72, // set this
        child:
            Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
          Icon(
            size: 14,
            _icon,
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [
              Text(
                value(_value),
                style: Theme.of(context).textTheme.bodyMedium,
              ),
              const SizedBox(width: 2),
              Text(
                _unit,
                style: Theme.of(context).textTheme.bodySmall,
                //textAlign: TextAlign.left,
              )
            ],
          ),
          const SizedBox(
            width: 6,
          )
        ]));
  }

  String value(double v) {
    if (v.abs() == 0) return v.abs().toStringAsFixed(0);
    if (v.abs() < 100) return v.abs().toStringAsFixed(1); // For temperature
    if (v.abs() < 1000) return v.abs().toStringAsFixed(0);

    return (v / 1000).abs().toStringAsFixed(1);
  }
}
