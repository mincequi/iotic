import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/ui/things/thing_property.dart';
import 'package:iotic/ui/things/thing_util.dart';

import '../../data/repository.dart';
import '../../data/properties.dart';
import '../../data/thing_property.dart';

class ThingCardController extends GetxController {
  ThingCardController(this._id);

  final name = "".obs;
  final icon = Icons.device_hub.obs;

  final isEditingMode = false.obs;
  final isPinned = false.obs;

  // Mutable properties
  final isOn = RxnBool();
  final offset = Rxn<int>();
  final status = Rxn<IconData>();

  final propertyWidgets = <ThingProperty, ThingUiProperty>{}.obs;

  @override
  void onReady() {
    assignProperties(_repo.things);
    _repo.things.listen((p0) {
      assignProperties(p0);
    });

    super.onReady();
  }

  @override
  void onClose() {
    _repo.things.close();
    super.onClose();
  }

  void assignProperties(Map<String, Properties> props) {
    var p0 = props[_id]?.properties;
    if (p0 == null) return;

    // Check for name
    if (p0.containsKey(ThingProperty.name) &&
        p0[ThingProperty.name].toString().isNotEmpty) {
      name.value = p0[ThingProperty.name];
    } else {
      name.value = _id;
    }

    // Check if thing isPinned
    if (p0.containsKey(ThingProperty.pinned)) {
      isPinned.value = p0[ThingProperty.pinned];
    }

    // Check for type
    icon.value = _typeToIcon[p0[ThingProperty.type]] ?? Icons.device_hub;
    offset.value = p0[ThingProperty.offset];
    status.value = toIcon(p0[ThingProperty.status]);

    // Check for power control
    //hasPowerControl.value = p0.containsKey(ReadableThingProperty.power_control);
    //if (hasPowerControl.value) {
    isOn.value = p0[ThingProperty.power_control];
    if (icon.value == Icons.device_hub) {
      icon.value = Icons.electrical_services;
    }
    //}

    // TODO: this has to be dynamic. Otherwise things cards won't get updated.
    // No idea why.
    dynamic p;
    if ((p = p0[ThingProperty.power]) != null) {
      propertyWidgets[ThingProperty.power] =
          ThingUiProperty(Icons.electric_bolt, p, _powerUnit(p.round()));
    }
    if ((p = p0[ThingProperty.temperature]) != null) {
      propertyWidgets[ThingProperty.temperature] =
          ThingUiProperty(Icons.thermostat, p, '°C');
    }
  }

  final Repository _repo = Get.find<Repository>();

  final String _id;

  static final Map<String?, IconData> _typeToIcon = {
    "smartMeter": Icons.electric_meter,
    "solarInverter": Icons.solar_power,
    "evStation": Icons.ev_station,
    null: Icons.device_hub
  };

  static String _powerUnit(int v) {
    if (v.abs() < 1000) return "W";
    return "kW";
  }
}
