import 'package:flutter/material.dart';

class IoticTheme {
  // Private Constructor
  IoticTheme._();

  static final ThemeData light = ThemeData(
    useMaterial3: true,
    //brightness: Brightness.light,
    colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange),
  );

  static final ThemeData dark = ThemeData(
    useMaterial3: true,
    brightness: Brightness.dark,
    //colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange),
    //primarySwatch: Colors.red,
    //errorColor: Colors.amber,
  );
}
