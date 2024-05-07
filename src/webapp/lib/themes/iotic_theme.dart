import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:google_fonts/google_fonts.dart';

import 'palette_v6.dart';
import 'palette_v7.dart';

class IoticTheme {
  // Private Constructor
  IoticTheme._();

  static const yellow = PaletteV6.yellow;
  static const orange = PaletteV6.orange;
  static const green = PaletteV6.green;
  static const blue = PaletteV7.blue;
  //static const other = PaletteV3.other;
  static const other = Color.fromARGB(255, 127, 127, 127);

  static ThemeData dark(BuildContext context) {
    final baseTheme = Theme.of(context);

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
            elevation: 0.0,
            color: Colors.black,
            shape: RoundedRectangleBorder(
              side: BorderSide(color: other, width: 1.0),
              borderRadius: BorderRadius.all(Radius.circular(4.0)),
            )),
        listTileTheme: const ListTileThemeData(
            horizontalTitleGap: 8.0,
            contentPadding: EdgeInsets.only(top: 0, left: 6.0, right: 8.0)),
        switchTheme: SwitchThemeData(
          trackColor: MaterialStateColor.resolveWith((states) {
            if (states.contains(MaterialState.selected))
              return IoticTheme.yellow;
            else
              return Colors.transparent;
          }),
          trackOutlineColor: MaterialStateColor.resolveWith((states) {
            if (states.contains(MaterialState.selected))
              return Colors.transparent;
            else
              return IoticTheme.other;
          }),
        ));
  }
}
