import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/preview/preview_data_source.dart';
import 'package:iotic/site/card/data/site_service.dart';
import 'package:iotic/site/site_page.dart';

void main() async {
  Get.put<WebSocketDataSource>(PreviewDataSource());
  Get.put<SiteService>(SiteService());
  runApp(const IoticApp());
}

class IoticApp extends StatelessWidget {
  const IoticApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return GetMaterialApp(
        title: 'iotic preview',
        darkTheme: IoticTheme.dark(context),
        themeMode: ThemeMode.dark,
        color: Colors.black,
        home: Scaffold(
          appBar: AppBar(
            backgroundColor: Colors.transparent,
            title: const Text("this is iotic preview"),
          ),
          body: SafeArea(
              child: Padding(
                  padding: const EdgeInsets.only(left: 8, right: 8),
                  child: SitePage())),
          backgroundColor: Colors.black,
        ));
  }
}
