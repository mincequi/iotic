import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/ui/things/thing_property.dart';

import '../../data/repository.dart';
import '../../data/thing_live_data.dart';

class ThingCardController extends GetxController {
  ThingCardController(this._id);

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

  final properties = <ReadableThingProperty, dynamic>{}.obs;
  final name = "".obs;
  final icon = Icons.device_hub.obs;
  final hasPowerControl = false.obs;
  final powerControl = false.obs;

  final propertyWidgets = <ReadableThingProperty, ThingProperty>{}.obs;

  @override
  void onReady() {
    //assignProperties(_repo.things);
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

  void assignProperties(Map<String, ThingLiveData> props) {
    var p0 = props[_id]?.properties;
    if (p0 != null) {
      properties.value = p0;

      name.value = p0[ReadableThingProperty.name] ?? _id;
      icon.value =
          _typeToIcon[p0[ReadableThingProperty.type]] ?? Icons.device_hub;

      hasPowerControl.value =
          p0.containsKey(ReadableThingProperty.powerControl);
      if (hasPowerControl.value) {
        powerControl.value = p0[ReadableThingProperty.powerControl] != 0;
        if (icon.value == Icons.device_hub) {
          icon.value = Icons.electrical_services;
        }
      }

      dynamic p;
      if ((p = p0[ReadableThingProperty.power]) != null) {
        propertyWidgets[ReadableThingProperty.power] =
            ThingProperty(Icons.electric_bolt, p, _powerUnit(p.round()));
      }
      if ((p = p0[ReadableThingProperty.temperature]) != null) {
        propertyWidgets[ReadableThingProperty.temperature] =
            ThingProperty(Icons.thermostat, p, '°C');
      }
    }
  }

  final Repository _repo = Get.find<Repository>();
}
