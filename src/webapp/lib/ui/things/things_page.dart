import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/ui/things/thing_card.dart';
import 'package:iotic/ui/things/things_page_controller.dart';

class ThingsPage extends StatelessWidget {
  ThingsPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Obx(() => ListView.builder(
          padding: const EdgeInsets.only(left: 8, right: 8),
          itemCount: _control.thingCount.value,
          itemBuilder: (BuildContext context, int index) {
            return ThingCard(_control.thingIds.elementAt(index));
          },
        ));
  }

  final ThingsPageController _control = Get.put(ThingsPageController());
}
