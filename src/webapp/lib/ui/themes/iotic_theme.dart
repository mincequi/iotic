import 'package:flutter/material.dart';
import 'package:iotic/ui/themes/palette_v1.dart';
import 'package:iotic/ui/themes/palette_v2.dart';
import 'package:iotic/ui/themes/palette_v3.dart';

class IoticTheme {
  // Private Constructor
  IoticTheme._();

  static final ThemeData light = ThemeData(
      useMaterial3: true,
      //brightness: Brightness.light,
      colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange));
  //colorScheme: const ColorScheme.dark(onPrimary: Colors.yellow));

  static const _yellow = PaletteV3.yellow;
  static const _orange = PaletteV3.orange;
  static const _green = PaletteV3.green;
  static const blue = PaletteV3.blue;
  static const other = PaletteV3.other;

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
          //margin: EdgeInsets.only(right: 4.0),
          color: other,
          surfaceTintColor: Colors.transparent,
          shape: RoundedRectangleBorder(
            //side: BorderSide(color: Colors.white, width: 1.0),
            borderRadius: BorderRadius.all(Radius.circular(18.0)),
            //borderRadius: BorderRadius.circular(20.0),
          )),
      listTileTheme: const ListTileThemeData(
          contentPadding: EdgeInsets.only(left: 18.0, right: 9.0))
      //primarySwatch: Colors.red,
      //errorColor: Colors.amber,
      );
}
