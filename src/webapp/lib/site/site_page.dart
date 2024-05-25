import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/things/card/thing_card_site.dart';

import 'card/site_card.dart';
import 'site_page_controller.dart';

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
          child: SiteCard()),
      Expanded(
          child: Obx(() => ListView.builder(
                itemCount: _control.thingCount.value,
                itemBuilder: (BuildContext context, int index) {
                  return ThingCardSite(_control.thingIds.elementAt(index));
                },
              )))
    ]);
  }
}
