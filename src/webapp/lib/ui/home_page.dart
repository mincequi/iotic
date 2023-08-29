import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/ui/pages/historic.dart';
import 'package:iotic/ui/pages/live.dart';
import 'package:iotic/ui/pages/site.dart';
import 'package:iotic/ui/things/things_page.dart';

import '../data/repository.dart';
import 'bottom_navigation_bar.dart';
import 'home_controller.dart';

class Home extends StatelessWidget {
  const Home({Key? key}) : super(key: key);

  static final _pages = [
    Site(),
    //const Live(),
    ThingsPage(),
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
        title: const Text("This is iotic"),
      ),
      body: Obx(() => _pages[c.currentPage.value]),
      bottomNavigationBar:
          Obx(() => MyBottomNavigationBar(c.currentPage.value)),
    );
  }
}
