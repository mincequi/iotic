import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_navigation/src/extension_navigation.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/data/repository.dart';

import '../themes/iotic_theme.dart';

class SiteReadoutsController extends GetxController {
  final repo = Get.find<Repository>();

  final pvPower = 0.obs;
  Color pvColor() {
    return pvPower.value > 0
        ? Theme.of(Get.context!).colorScheme.secondary
        : Theme.of(Get.context!).dividerColor;
  }

  final sitePower = 0.obs;
  final gridPower = 0.obs;
  Color gridColor() {
    return gridPower.value >= 0
        ? Theme.of(Get.context!).colorScheme.tertiary
        : IoticTheme.blue;
  }

  @override
  void onReady() {
    repo.siteLiveData.listen((p0) {
      //int r = Random().nextInt(200) + 900;
      pvPower.value = p0.pvPower;
      sitePower.value = p0.sitePower;
      gridPower.value = p0.gridPower;
    });
    super.onReady();
  }

  @override
  void onClose() {
    repo.siteLiveData.close();
    super.onClose();
  }
}
