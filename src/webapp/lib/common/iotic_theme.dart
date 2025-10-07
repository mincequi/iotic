import 'package:flutter/material.dart';
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
  static const other = Color.fromARGB(255, 102, 102, 102);
  static const rectangleRadius = 3.0;
  static const lineWidth = 1.0;
  static const shadowOffset = 2.0;

  static ThemeData dark(BuildContext context) {
    var baseTheme = ThemeData.dark(useMaterial3: true);

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
      cardTheme: const CardThemeData(
          elevation: 0.0,
          color: Colors.black,
          shape: RoundedRectangleBorder(
            side: BorderSide(color: other, width: lineWidth),
            borderRadius: BorderRadius.all(Radius.circular(rectangleRadius)),
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
        trackOutlineWidth: MaterialStateProperty.resolveWith((states) {
          return lineWidth;
        }),
        thumbColor: MaterialStateColor.resolveWith((states) {
          if (states.contains(MaterialState.selected))
            return Colors.black;
          else
            return IoticTheme.yellow;
        }),
      ),
      bottomNavigationBarTheme: const BottomNavigationBarThemeData(
        backgroundColor: Colors.black,
        elevation: 0,
        selectedItemColor: yellow,
        unselectedItemColor: other,
        selectedLabelStyle: TextStyle(
          color: yellow,
          fontSize: 12,
          fontWeight: FontWeight.w500,
        ),
        unselectedLabelStyle: TextStyle(
          color: other,
          fontSize: 12,
          fontWeight: FontWeight.w500,
        ),
      ),
      navigationBarTheme: const NavigationBarThemeData(
        backgroundColor: Colors.black,
        elevation: 0,
        shadowColor: null,
        surfaceTintColor: null,
        indicatorColor: IoticTheme.yellow,
        indicatorShape: null,
        labelBehavior: NavigationDestinationLabelBehavior.alwaysShow,
      ),
      //textTheme: GoogleFonts.robotoTextTheme(baseTheme.textTheme),
    );
  }
}
