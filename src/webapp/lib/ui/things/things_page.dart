import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/data/repository.dart';
import 'package:iotic/data/thing_live_data.dart';
import 'package:iotic/ui/things/thing_card.dart';
import 'package:iotic/ui/things/things_controller.dart';
//import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

class ThingsPage extends StatelessWidget {
  //final SiteDemoController c = Get.put(SiteDemoController());
  //final ThingsController c = Get.put(ThingsController());
  final repo = Get.find<Repository>();

  ThingsPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Obx(() => ListView.builder(
          padding: const EdgeInsets.only(left: 8, right: 8),
          itemCount: repo.things.length,
          itemBuilder: (BuildContext context, int index) {
            var kv = repo.things.entries.elementAt(index);
            return ThingCard(kv.key, kv.value.properties);
          },
        ));
  }
}
