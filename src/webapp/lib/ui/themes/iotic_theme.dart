import 'package:flutter/material.dart';
import 'package:iotic/ui/themes/palette_v1.dart';
import 'package:iotic/ui/themes/palette_v2.dart';

class IoticTheme {
  // Private Constructor
  IoticTheme._();

  static final ThemeData light = ThemeData(
      useMaterial3: true,
      //brightness: Brightness.light,
      colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange));
  //colorScheme: const ColorScheme.dark(onPrimary: Colors.yellow));

  // https://www.pinterest.com/pin/317433473719060822/
  static const _yellow = PaletteV2.yellow;
  //static const _orange = PaletteV1.orange;
  static const _orange = PaletteV2.orange;
  static const _green = PaletteV2.green;
  static const blue = PaletteV2.blue;
  //static const other = PaletteV1.other;

  static final ThemeData dark = ThemeData(
      useMaterial3: true,
      brightness: Brightness.dark,
      /*colorScheme: const ColorScheme.dark(
          primary: _yellow, secondary: _green, tertiary: _orange),*/
      colorScheme: ColorScheme.fromSeed(
          seedColor: _yellow,
          brightness: Brightness.dark,
          secondary: _green,
          tertiary: _orange),
      cardTheme: const CardTheme(
          color: Color.fromARGB(255, 49, 51, 52),
          surfaceTintColor: Colors.transparent)
      //primarySwatch: Colors.red,
      //errorColor: Colors.amber,
      );
}
