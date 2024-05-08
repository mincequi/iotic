import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_navigation/get_navigation.dart';
import 'package:iotic/data/logs/logging_service.dart';
import 'package:iotic/data/site/site_repository.dart';
import 'package:iotic/data/things/thing_repository.dart';

import 'ui/home_page.dart';
import 'ui/iotic_theme.dart';
import 'data/site/site_service.dart';
import 'data/web_socket_data_source.dart';

void main() async {
  // Init LoggingService first since it is potentially used by any other component
  Get.put(LoggingService());
  // Init the WebSocketDataSource and its dependencees
  Get.put(WebSocketDataSource());
  Get.put(ThingRepository(Get.find<WebSocketDataSource>()));
  Get.put(SiteRepository(Get.find<WebSocketDataSource>()));
  Get.put<SiteService>(SiteService());
  runApp(const IoticApp());
}

class IoticApp extends StatelessWidget {
  const IoticApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return GetMaterialApp(
      title: 'iotic',
      darkTheme: IoticTheme.dark(context),
      themeMode: ThemeMode.dark,
      color: Colors.black,
      home: const Home(),
    );
  }
}
