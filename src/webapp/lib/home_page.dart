import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';

import 'home_navigation_bar.dart';
import 'home_page_controller.dart';
import 'logs/logs_page.dart';
import 'settings/settings_page.dart';
import 'site/site_page.dart';
import 'things/things_page.dart';

class Home extends StatelessWidget {
  const Home({Key? key}) : super(key: key);

  static final _pages = [
    SitePage(),
    //const Live(),
    ThingsPage(),
    SettingsPage(),
    LogsPage(),
    //const Historic(),
  ];

  @override
  Widget build(context) {
    // Instantiate your class using Get.put() to make it available for all "child" routes there.
    final HomePageController c = Get.put(HomePageController());

    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.transparent,
        title: const Text("this is iotic"),
      ),
      body: SafeArea(
          child: Padding(
              padding: const EdgeInsets.only(left: 8, right: 8),
              child: Obx(() => _pages[c.currentPage.value]))),
      bottomNavigationBar: Padding(
          padding: const EdgeInsets.only(bottom: 34),
          child: Obx(() => HomeNavigationBar(c.currentPage.value))),
      backgroundColor: Colors.black,
    );
  }
}
