import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_navigation/get_navigation.dart';

import 'home_page.dart';
import 'common/data/web_socket_data_source.dart';
import 'site/application/site_service.dart';
import 'themes/iotic_theme.dart';

void main() async {
  Get.put(WebSocketDataSource());
  Get.put<SiteService>(SiteService());
  runApp(const IoticApp());
}

class IoticApp extends StatelessWidget {
  const IoticApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return GetMaterialApp(
      title: 'iotic',
      theme: IoticTheme.light,
      darkTheme: IoticTheme.dark(context),
      themeMode: ThemeMode.dark,
      color: Colors.black,
      home: const Home(),
    );
  }
}
