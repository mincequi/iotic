import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/ui/site/site_card.dart';
import 'package:iotic/ui/site/site_page_controller.dart';
import 'package:iotic/ui/site/site_thing_card.dart';

class SitePage extends StatelessWidget {
  final _control = Get.put(SitePageController());

  SitePage({super.key});

  @override
  Widget build(BuildContext context) {
    return Column(children: [
      SizedBox(
          //height: 266,
          height: 194,
          width: double.infinity,
          child: Padding(
              padding: const EdgeInsets.only(left: 8, right: 8),
              child: SiteCard())),
      Expanded(
          child: Obx(() => ListView.builder(
                padding: const EdgeInsets.only(left: 8, right: 8),
                itemCount: _control.thingCount.value,
                itemBuilder: (BuildContext context, int index) {
                  return SiteThingCard(_control.thingIds.elementAt(index));
                },
              )))
    ]);
  }
}
