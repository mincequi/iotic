import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart' show Get;
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';
import 'package:iotic/settings/settings_card.dart';
import 'package:iotic/settings/settings_page_controller.dart';

class SettingsPage extends StatelessWidget {
  SettingsPage({super.key});

  final SettingsPageController _control = Get.put(SettingsPageController());

  @override
  Widget build(BuildContext context) {
    return Obx(() => ListView.builder(
          itemCount: _control.sections.length,
          itemBuilder: (BuildContext context, int index) {
            return SettingsCard(_control.sections[index]);
          },
        ));
  }
}
