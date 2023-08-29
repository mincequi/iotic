import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';

import 'home_controller.dart';

class MyBottomNavigationBar extends StatelessWidget {
  final int _currentIndex;

  final c = Get.find<HomeController>();

  MyBottomNavigationBar(this._currentIndex, {super.key});

  @override
  Widget build(context) {
    return BottomNavigationBar(
      items: const <BottomNavigationBarItem>[
        BottomNavigationBarItem(
          icon: Icon(Icons.line_axis),
          label: 'Site',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.library_books),
          label: 'Things',
        ),
        /*BottomNavigationBarItem(
          icon: Icon(Icons.multiline_chart_outlined),
          label: 'Historic',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.settings),
          label: 'Settings',
        ),
        */
      ],
      currentIndex: _currentIndex,
      selectedItemColor: Colors.yellow,
      unselectedItemColor: Colors.grey,
      onTap: (value) {
        c.currentPage.value = value;
      },
    );
  }
}
