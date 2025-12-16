import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/common/web_socket_data_source.dart';

import '../data/thing_properties.dart';
import '../data/thing_property.dart';
import 'thing_ui_property.dart';
import 'thing_util.dart';

class ThingCardController extends GetxController {
  ThingCardController(this._id);
  final String _id;

  final WebSocketDataSource _repo = Get.find<WebSocketDataSource>();

  final name = "".obs;
  final icon = Icons.device_hub.obs;

  final isEditingMode = false.obs;
  final isPinned = false.obs;

  final isOn = RxnBool();
  final offset = Rxn<int>();
  final status = Rxn<IconData>();
  final countdown = Rxn<int>();
  final voltages = Rxn<List<int>>();

  final propertyWidgets = <ThingUiProperty>[].obs;

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

  void assignProperties(Map<String, ThingProperties> props) {
    var p0 = props[_id]?.properties;
    if (p0 == null) return;

    // Check for name
    if (p0.containsKey(ThingPropertyKey.name) &&
        p0[ThingPropertyKey.name].toString().isNotEmpty) {
      name.value = p0[ThingPropertyKey.name];
    } else {
      name.value = _id;
    }

    // Check if thing isPinned
    if (p0.containsKey(ThingPropertyKey.pinned)) {
      isPinned.value = p0[ThingPropertyKey.pinned];
    }

    // Check for type
    icon.value = _typeToIcon[p0[ThingPropertyKey.type]] ?? Icons.device_hub;
    offset.value = p0[ThingPropertyKey.offset];
    status.value = toIcon(p0[ThingPropertyKey.status]);
    countdown.value = p0[ThingPropertyKey.countdown];
    voltages.value = p0[ThingPropertyKey.voltage]?.cast<int>();

    // Check for power control
    //hasPowerControl.value = p0.containsKey(ReadableThingProperty.power_control);
    //if (hasPowerControl.value) {
    isOn.value = p0[ThingPropertyKey.power_control];
    if (icon.value == Icons.device_hub) {
      icon.value = Icons.electrical_services;
    }
    //}

    // TODO: this has to be dynamic. Otherwise things cards won't get updated.
    // No idea why.
    dynamic p;
    propertyWidgets.clear();
    if ((p = p0[ThingPropertyKey.power]) != null) {
      propertyWidgets.add(//[ThingPropertyKey.power] =
          ThingUiProperty(Icons.electric_bolt, p, _powerUnit(p.round()),
              ThingPropertyKey.power));
    }
    if ((p = p0[ThingPropertyKey.temperature]) != null) {
      propertyWidgets.add(//[ThingPropertyKey.temperature] =
          ThingUiProperty(
              Icons.thermostat, p / 10.0, '°C', ThingPropertyKey.temperature));
    }
  }

  static final Map<String?, IconData> _typeToIcon = {
    "smartMeter": Icons.electric_meter,
    "solarInverter": Icons.solar_power,
    "evStation": Icons.ev_station,
    "weatherStation": Icons.local_fire_department,
    //"heatingSystem": Icons.fireplace,
    null: Icons.device_hub
  };

  static String _powerUnit(int v) {
    if (v.abs() < 1000) return "W";
    return "kW";
  }
}
