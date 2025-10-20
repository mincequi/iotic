import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';

import 'home_page_controller.dart';

class HomeNavigationBar extends StatelessWidget {
  final int _currentIndex;

  final _control = Get.find<HomePageController>();

  HomeNavigationBar(this._currentIndex, {super.key});

  @override
  Widget build(context) {
    return BottomNavigationBar(
      type: BottomNavigationBarType.fixed,
      items: const <BottomNavigationBarItem>[
        BottomNavigationBarItem(
          icon: Icon(Icons.line_axis),
          label: 'site',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.library_books),
          label: 'things',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.settings),
          label: 'settings',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.monitor_sharp),
          label: 'insights',
        ),
      ],
      currentIndex: _currentIndex,
      onTap: (value) {
        _control.currentPage.value = value;
      },
    );
  }
}
