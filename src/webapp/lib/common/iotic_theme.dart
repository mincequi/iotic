import 'dart:math';

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
        tertiary: orange,
      ),
      cardTheme: const CardThemeData(
        elevation: 0.0,
        color: Colors.black,
        shape: RoundedRectangleBorder(
          side: BorderSide(color: other, width: lineWidth),
          borderRadius: BorderRadius.all(Radius.circular(rectangleRadius)),
        ),
      ),
      listTileTheme: const ListTileThemeData(
        horizontalTitleGap: 8.0,
        contentPadding: EdgeInsets.only(top: 0, left: 6.0, right: 8.0),
      ),
      switchTheme: SwitchThemeData(
        trackOutlineColor: WidgetStateColor.resolveWith((states) {
          if (states.contains(WidgetState.selected)) {
            return Colors.transparent;
          } else {
            return IoticTheme.other;
          }
        }),
        trackOutlineWidth: WidgetStateProperty.resolveWith((states) {
          return lineWidth;
        }),
        trackColor: WidgetStateColor.resolveWith((states) {
          if (states.contains(WidgetState.selected)) {
            return IoticTheme.yellow;
          } else {
            return Colors.transparent;
          }
        }),
        thumbColor: WidgetStateColor.resolveWith((states) {
          if (states.contains(WidgetState.selected)) {
            return Colors.black;
          } else {
            return IoticTheme.yellow;
          }
        }),
      ),
      segmentedButtonTheme: SegmentedButtonThemeData(
        style: ButtonStyle(
          // Background of each segment
          backgroundColor: WidgetStateProperty.resolveWith((states) {
            if (states.contains(WidgetState.selected)) {
              if (states.contains(WidgetState.disabled)) {
                return IoticTheme.other;
              }
              return IoticTheme.yellow;
            }
            return Colors.transparent;
          }),

          // Border (outline)
          side: WidgetStateProperty.resolveWith((states) {
            return const BorderSide(
              color: IoticTheme.other,
              width: lineWidth,
            );
          }),

          // Text / icon color
          foregroundColor: WidgetStateProperty.resolveWith((states) {
            if (states.contains(WidgetState.selected)) {
              return Colors.black;
            } else if (states.contains(WidgetState.disabled)) {
              return IoticTheme.other;
            }
            return IoticTheme.yellow;
          }),

          // Optional: pressed/hover overlay
          overlayColor: WidgetStateProperty.resolveWith((states) {
            if (states.contains(WidgetState.pressed)) {
              return IoticTheme.yellow.withOpacity(0.15);
            }
            return null;
          }),

          // Optional: shape consistency
          shape: WidgetStateProperty.all(
            RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(8),
            ),
          ),
        ),
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
