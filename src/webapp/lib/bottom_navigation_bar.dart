import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';

import 'home_controller.dart';

class MyBottomNavigationBar extends StatelessWidget {
  final int _currentIndex;

  final _control = Get.find<HomeController>();

  MyBottomNavigationBar(this._currentIndex, {super.key});

  @override
  Widget build(context) {
    return BottomNavigationBar(
      backgroundColor: Colors.black,
      items: const <BottomNavigationBarItem>[
        BottomNavigationBarItem(
          icon: Icon(Icons.line_axis),
          label: 'site',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.library_books),
          label: 'things',
        ),
        /*BottomNavigationBarItem(
          icon: Icon(Icons.multiline_chart_outlined),
          label: 'Historic',
        ),*/
        BottomNavigationBarItem(
          icon: Icon(Icons.settings),
          label: 'settings',
        ),
      ],
      currentIndex: _currentIndex,
      // TODO: why do i need to set this manually? It seems to use secondary per default
      selectedItemColor: Theme.of(context).colorScheme.primary,
      //unselectedItemColor: Theme.of(context).colorScheme.secondary,
      onTap: (value) {
        _control.currentPage.value = value;
      },
    );
  }
}
