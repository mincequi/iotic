import 'package:flutter/material.dart';

const _offset = 1.5;
const _blurRadius = 0.1;
shadows() => [
      const Shadow(offset: Offset(-_offset, 0.0), blurRadius: _blurRadius),
      const Shadow(offset: Offset(0.0, -_offset), blurRadius: _blurRadius),
      const Shadow(offset: Offset(_offset, 0.0), blurRadius: _blurRadius),
      const Shadow(offset: Offset(0.0, _offset), blurRadius: _blurRadius),
      //Shadow(offset: Offset(0.0, ), blurRadius: _blurRadius),
    ];

textStyle(Color color, {FontWeight fontWeight = FontWeight.bold}) => TextStyle(
    color: color, fontWeight: fontWeight, fontSize: 12, shadows: shadows());

String powerUnit(int v) {
  if (v.abs() < 1000) return "W";
  return "kW";
}

String value(int v) {
  if (v.abs() < 1000) return v.toDouble().abs().toString();

  return (v / 1000.0).abs().round().toStringAsFixed(1);
}
