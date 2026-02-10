import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/common/web_socket_data_source.dart';
//import 'package:iotic/things/components/meter.dart';

import '../data/thing_properties.dart';
import '../data/thing_property.dart';
import 'thing_util.dart';

class ThingCardController extends GetxController {
  ThingCardController(this._id);
  final String _id;

  final WebSocketDataSource _dataSource = Get.find<WebSocketDataSource>();

  final name = "".obs;
  String type = "";
  final icon = Icons.device_hub.obs;

  final isEditingMode = false.obs;
  final isPinned = false.obs;

  final isOn = RxnBool();
  final offset = Rxn<int>();
  final status = Rxn<IconData>();
  final countdown = Rxn<int>();
  final voltages = Rxn<List<int>>();
  final multistateSelector = Rxn<List<bool>>();
  final digitalInput = Rxn<List<bool>>();

  final power = Rxn<int>();
  final temperature = Rxn<int>();
  final energy = Rxn<double>();

  final hasSubtitle = false.obs;

  @override
  void onReady() {
    assignProperties(_dataSource.things);
    _dataSource.things.listen((p0) {
      assignProperties(p0);
    });

    super.onReady();
  }

  @override
  void onClose() {
    _dataSource.things.close();
    super.onClose();
  }

  void assignProperties(Map<String, ThingProperties> props) {
    var p0 = props[_id]?.properties;
    if (p0 == null) return;

    // Name
    if (p0.containsKey(ThingPropertyKey.name) &&
        p0[ThingPropertyKey.name].toString().isNotEmpty) {
      name.value = p0[ThingPropertyKey.name];
    } else {
      name.value = _id;
    }
    // isPinned
    if (p0.containsKey(ThingPropertyKey.pinned)) {
      isPinned.value = p0[ThingPropertyKey.pinned];
    }
    // multistateSelector
    if (p0.containsKey(ThingPropertyKey.multistateSelector)) {
      multistateSelector.value =
          (p0[ThingPropertyKey.multistateSelector] as List?)?.cast<bool>();
    }
    // digitalInout
    if (p0.containsKey(ThingPropertyKey.digitalInput)) {
      digitalInput.value =
          (p0[ThingPropertyKey.digitalInput] as List?)?.cast<bool>();
    }

    // Check for type
    type = p0[ThingPropertyKey.type] ?? "relay";
    icon.value = _typeToIcon[p0[ThingPropertyKey.type]] ?? Icons.device_hub;
    offset.value = p0[ThingPropertyKey.offset];
    status.value = toIcon(p0[ThingPropertyKey.status]);
    countdown.value = p0[ThingPropertyKey.countdown];
    voltages.value = p0[ThingPropertyKey.voltage]?.cast<int>();

    // Check for power control
    //hasPowerControl.value = p0.containsKey(ReadableThingProperty.power_control);
    //if (hasPowerControl.value) {
    isOn.value = p0[ThingPropertyKey.power_control];
    if (isOn.value != null) {
      icon.value = Icons.electrical_services;
    }
    //}

    power.value = p0[ThingPropertyKey.power];
    temperature.value = p0[ThingPropertyKey.temperature];
    energy.value = p0[ThingPropertyKey.energy];

    hasSubtitle.value = power.value != null ||
        temperature.value != null ||
        energy.value != null;
  }

  static final Map<String?, IconData> _typeToIcon = {
    "smartMeter": Icons.electric_meter,
    "solarInverter": Icons.solar_power,
    "evStation": Icons.ev_station,
    "relay": Icons.electrical_services,
    "weatherStation": Icons.local_fire_department,
    "heater": Icons.local_fire_department,
    null: Icons.device_hub
  };
}
