import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/common/web_socket_data_source.dart';

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
  final voltages = List<RxInt>.empty(growable: true).obs;
  final multistateSelector = Rxn<List<bool>>();
  final digitalInput = Rxn<List<bool>>();
  final isEnabled = false.obs;
  final dcPower = List<RxInt>.empty(growable: true);
  final dcEnergy = List<RxDouble>.empty(growable: true);

  final power = RxnNum();
  final temperature = RxnNum();
  RxDouble? energy;

  final hasSubtitle = false.obs;

  @override
  void onReady() {
    assignProperties(_dataSource.things);
    _dataSource.things.listen((p0) {
      assignProperties(p0);
    });

    digitalInput.listen((value) {
      isEnabled.value = value?.every((element) => element) ?? false;
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
    // dcPower
    if (p0.containsKey(ThingPropertyKey.dcPower)) {
      while (dcPower.length < (p0[ThingPropertyKey.dcPower] as List).length) {
        dcPower.add(RxInt(0));
      }
      for (int i = 0; i < (p0[ThingPropertyKey.dcPower] as List).length; i++) {
        dcPower[i].value = (p0[ThingPropertyKey.dcPower] as List)[i];
      }
      //dcPower[0].value = 8400;
      //dcPower[1].value = 6000;
    }
    // dcEnergy
    if (p0.containsKey(ThingPropertyKey.dcEnergy)) {
      while (dcEnergy.length < (p0[ThingPropertyKey.dcEnergy] as List).length) {
        dcEnergy.add(RxDouble(0));
      }
      for (int i = 0; i < (p0[ThingPropertyKey.dcEnergy] as List).length; i++) {
        dcEnergy[i].value =
            (p0[ThingPropertyKey.dcEnergy] as List)[i].toDouble() / 60.0;
      }
    }

    // Check for type
    type = p0[ThingPropertyKey.type] ?? "relay";
    icon.value = _typeToIcon[p0[ThingPropertyKey.type]] ?? Icons.device_hub;
    offset.value = p0[ThingPropertyKey.offset];
    status.value = toIcon(p0[ThingPropertyKey.status]);
    countdown.value = p0[ThingPropertyKey.countdown];

    if (p0.containsKey(ThingPropertyKey.voltage)) {
      while (voltages.length < (p0[ThingPropertyKey.voltage] as List).length) {
        voltages.add(RxInt(0));
      }
      for (int i = 0; i < (p0[ThingPropertyKey.voltage] as List).length; i++) {
        voltages[i].value = (p0[ThingPropertyKey.voltage] as List)[i];
      }

      //voltages[0].value = 33;
      //voltages[1].value = 55;
    }

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

    if (p0[ThingPropertyKey.energy] != null) {
      energy ??= RxDouble(0);
      energy!.value = (p0[ThingPropertyKey.energy] as num).toDouble() / 60.0;
    }

    hasSubtitle.value = power.value != null ||
        temperature.value != null ||
        energy != null ||
        dcEnergy.isNotEmpty;
  }

  static final Map<String?, IconData> _typeToIcon = {
    "smartMeter": Icons.electric_meter,
    "solarInverter": Icons.solar_power,
    "evStation": Icons.ev_station,
    "relay": Icons.electrical_services,
    "weatherStation": Icons.local_fire_department,
    "heater": Icons.local_fire_department,
    null: Icons.device_hub,
  };
}
