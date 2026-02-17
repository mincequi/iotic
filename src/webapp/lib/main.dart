import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_navigation/get_navigation.dart';
import 'package:iotic/logs/log_service.dart';
import 'package:iotic/site/card/data/site_repository.dart';

import 'home_page.dart';
import 'common/iotic_theme.dart';
import 'site/card/data/site_service.dart';
import 'common/web_socket_data_source.dart';

void main() async {
  // Init LoggingService first since it is potentially used by any other component
  Get.put(LogService());
  // Init the WebSocketDataSource and its dependencees
  Get.put(WebSocketDataSource());
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
      home: Home(),
    );
  }
}
