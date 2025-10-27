import 'package:flutter/material.dart';
import 'package:iotic/common/text_util.dart';
//import 'package:google_fonts/google_fonts.dart';

class ValueUnit extends StatelessWidget {
  const ValueUnit(this._icon, this._value, this._color, {super.key});

  final IconData _icon;
  final int _value;
  final Color _color;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
        width: 56, // set this
        child:
            Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
          Icon(
            size: 18,
            _icon,
            color: _color.withAlpha(200),
            shadows: shadows(),
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [
              Text(
                value(_value),
                style: textStyle(_color),
                //textAlign: TextAlign.right,
              ),
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
