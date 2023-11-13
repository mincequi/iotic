import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/ui/settings/settings_page.dart';
import 'package:iotic/ui/site/site_page.dart';
import 'package:iotic/ui/things/things_page.dart';

import '../data/repository.dart';
import 'bottom_navigation_bar.dart';
import 'home_controller.dart';

class Home extends StatelessWidget {
  const Home({Key? key}) : super(key: key);

  static final _pages = [
    SitePage(),
    //const Live(),
    ThingsPage(),
    SettingsPage()
    //const Historic(),
  ];

  @override
  Widget build(context) {
    // Instantiate your class using Get.put() to make it available for all "child" routes there.
    final HomeController c = Get.put(HomeController());

    // ignore: unused_local_variable
    final Repository repo = Get.put(Repository());

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
          child: Obx(() => MyBottomNavigationBar(c.currentPage.value))),
      backgroundColor: Colors.black,
    );
  }
}
