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
              ),
              const SizedBox(width: 2),
              Text(
                _unit,
                style: const TextStyle(
                  //color: color.withOpacity(0.6),
                  //fontWeight: FontWeight.bold,
                  fontSize: 12,
                ),
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
    if (v.abs() < 100) return v.abs().toStringAsFixed(1);
    if (v.abs() < 1000) return v.abs().toStringAsFixed(0);

    return (v / 1000).abs().toStringAsFixed(1);
  }
}
