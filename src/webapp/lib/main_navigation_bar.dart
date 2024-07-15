import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';

import 'main_page_controller.dart';

class MainNavigationBar extends StatelessWidget {
  final int _currentIndex;

  final _control = Get.find<MainPageController>();

  MainNavigationBar(this._currentIndex, {super.key});

  @override
  Widget build(context) {
    return BottomNavigationBar(
      type: BottomNavigationBarType.fixed,
      items: const <BottomNavigationBarItem>[
        BottomNavigationBarItem(
          icon: Icon(Icons.line_axis_sharp),
          label: 'site',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.devices_other_sharp),
          label: 'things',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.rule_sharp),
          label: 'rules',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.settings_sharp),
          label: 'settings',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.list_sharp),
          label: 'logs',
        ),
      ],
      currentIndex: _currentIndex,
      onTap: (value) {
        _control.currentPage.value = value;
      },
    );
  }
}
