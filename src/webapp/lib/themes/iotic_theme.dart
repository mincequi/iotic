import 'package:flutter/material.dart';

import 'palette_v6.dart';
import 'palette_v7.dart';

class IoticTheme {
  // Private Constructor
  IoticTheme._();

  static final ThemeData light = ThemeData(
      useMaterial3: true,
      //brightness: Brightness.light,
      colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange));
  //colorScheme: const ColorScheme.dark(onPrimary: Colors.yellow));

  static const yellow = PaletteV6.yellow;
  static const orange = PaletteV6.orange;
  static const green = PaletteV6.green;
  static const blue = PaletteV7.blue;
  //static const other = PaletteV3.other;
  static const other = Color.fromARGB(255, 33, 33, 33);

  static ThemeData dark(BuildContext context) {
    return ThemeData(
        useMaterial3: true,
        brightness: Brightness.dark,
        /*colorScheme: const ColorScheme.dark(
                primary: _yellow, secondary: _green, tertiary: _orange),*/
        colorScheme: ColorScheme.fromSeed(
            seedColor: yellow,
            brightness: Brightness.dark,
            secondary: green,
            tertiary: orange),
        cardTheme: const CardTheme(
            //margin: EdgeInsets.only(right: 4.0),
            color: other,
            surfaceTintColor: Colors.transparent,
            shape: RoundedRectangleBorder(
              //side: BorderSide(color: Colors.black, width: 1.0),
              borderRadius: BorderRadius.all(Radius.circular(8.0)),
              //borderRadius: BorderRadius.circular(20.0),
            )),
        listTileTheme: const ListTileThemeData(
            contentPadding: EdgeInsets.only(left: 16.0, right: 8.0)),
        switchTheme: SwitchThemeData(
            trackOutlineColor:
                MaterialStateColor.resolveWith((states) => Colors.transparent))
        //fontFamily: GoogleFonts.ub
        //textTheme: GoogleFonts.ubuntuTextTheme(baseTheme.textTheme)
        //primarySwatch: Colors.red,
        //errorColor: Colors.amber,
        );
  }
}
