import 'package:flutter/material.dart';

class ThingUiProperty extends StatelessWidget {
  ThingUiProperty(this._icon, this._value, this._unit,
      {this.color, super.key}) {}

  final IconData _icon;
  final num _value;
  final String _unit;
  final Color? color;

  @override
  Widget build(BuildContext context) {
    return /*SizedBox(
        width: 72, // set this
        child:*/
        Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
      Icon(
        size: 14,
        _icon,
        color: color,
      ),
      //const SizedBox(width: 3),
      SizedBox(
          width: 24,
          child: Align(
            alignment: Alignment.centerRight,
            child: Text(
              value(_value),
              style: Theme.of(context)
                  .textTheme
                  .bodyMedium
                  ?.copyWith(color: color, letterSpacing: -0.5),
            ),
          )),
      const SizedBox(width: 2),
      Text(
        _unit,
        style: Theme.of(context).textTheme.bodySmall?.copyWith(color: color),
        //textAlign: TextAlign.left,
      ),
    ]);
  }

  String value(num v) {
    if (v.abs() == 0) return v.abs().toStringAsFixed(0);
    if (v.abs() < 10) return v.abs().toStringAsFixed(1);
    if (v.abs() < 1000) return v.abs().toStringAsFixed(0);
    if (v.abs() < 10000) return (v / 1000).abs().toStringAsFixed(1);
    return (v / 1000).abs().toStringAsFixed(0);
  }
}
