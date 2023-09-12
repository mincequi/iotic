import 'package:flutter/material.dart';

class IoticTheme {
  // Private Constructor
  IoticTheme._();

  static final ThemeData light = ThemeData(
      useMaterial3: true,
      //brightness: Brightness.light,
      colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange));
  //colorScheme: const ColorScheme.dark(onPrimary: Colors.yellow));

  // https://www.pinterest.com/pin/317433473719060822/
  static const _yellow = Color.fromRGBO(247, 206, 92, 1.0);
  static const _orange = Color.fromRGBO(236, 133, 50, 1.0);
  static const _orange2 = Color.fromRGBO(247, 164, 92, 1);
  static const _green = Color.fromRGBO(166, 191, 142, 1.0);
  static const blue = Color.fromRGBO(135, 178, 176, 1.0);
  static const other = Color.fromRGBO(211, 222, 211, 1.0);

  static final ThemeData dark = ThemeData(
      useMaterial3: true,
      brightness: Brightness.dark,
      /*colorScheme: const ColorScheme.dark(
          primary: _yellow, secondary: _green, tertiary: _orange),*/
      colorScheme: ColorScheme.fromSeed(
          seedColor: _yellow,
          brightness: Brightness.dark,
          secondary: _green,
          tertiary: _orange2),
      cardTheme: const CardTheme(
          color: Color.fromARGB(255, 49, 51, 52),
          surfaceTintColor: Colors.transparent)
      //primarySwatch: Colors.red,
      //errorColor: Colors.amber,
      );
}
