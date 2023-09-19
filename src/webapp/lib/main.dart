import 'ui/home_page.dart';
import 'ui/themes/iotic_theme.dart';
import 'package:flutter/material.dart';
import 'package:get/get_navigation/get_navigation.dart';

void main() async {
  runApp(const IoticApp());
}

class IoticApp extends StatelessWidget {
  const IoticApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return GetMaterialApp(
      title: 'iotic',
      theme: IoticTheme.light,
      darkTheme: IoticTheme.dark,
      themeMode: ThemeMode.dark,
      color: Colors.black,
      /* ThemeMode.system to follow system theme, 
         ThemeMode.light for light theme, 
         ThemeMode.dark for dark theme
      */
      home: const Home(),
    );
  }
}
