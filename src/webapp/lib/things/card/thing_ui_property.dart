import 'package:flutter/material.dart';
import 'package:iotic/things/data/thing_property.dart';

class ThingUiProperty extends StatelessWidget {
  const ThingUiProperty(this._icon, this._value, this._unit, this._propertyKey,
      {this.color, super.key});

  final IconData _icon;
  final num _value;
  final String _unit;
  final ThingPropertyKey _propertyKey;
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
          width: 25,
          child: Align(
              alignment: Alignment.centerRight,
              child: Text(
                value(_value),
                style: Theme.of(context)
                    .textTheme
                    .bodyMedium
                    ?.copyWith(color: color),
              ))),
      const SizedBox(width: 2),
      Text(
        _unit,
        style: Theme.of(context).textTheme.bodySmall?.copyWith(color: color),
        //textAlign: TextAlign.left,
      ),
      const SizedBox(
        width: 12,
      )
    ]);
  }

  String value(num v) {
    if (v.abs() == 0) return v.abs().toStringAsFixed(0);
    if (v.abs() < 10) return v.abs().toStringAsFixed(1);
    if (v.abs() < 1000) return v.abs().toStringAsFixed(0);
    return (v / 1000).abs().toStringAsFixed(1);
  }
}
