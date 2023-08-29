import 'package:flutter/material.dart';

class ValueUnit extends StatelessWidget {
  const ValueUnit(this._icon, this._value, this._color, {super.key});

  final IconData _icon;
  final int _value;
  final Color _color;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
        width: 80, // set this
        child:
            Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
          Icon(
            size: 18,
            _icon,
            color: _color,
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [
              Text(
                value(_value),
                style: TextStyle(
                  color: _color,
                  fontWeight: FontWeight.bold,
                ),
                //textAlign: TextAlign.right,
              ),
              const SizedBox(width: 2),
              Text(
                unit(_value),
                style: TextStyle(
                  color: _color.withOpacity(0.6),
                  fontWeight: FontWeight.bold,
                  fontSize: 12,
                ),
                //textAlign: TextAlign.left,
              )
            ],
          )
        ]));
  }

  String value(int v) {
    if (v.abs() < 1000) return v.toDouble().abs().toString();

    return (v / 1000).abs().toStringAsFixed(1);
  }

  String unit(int v) {
    if (v.abs() < 1000) return "W";
    return "kW";
  }
}
