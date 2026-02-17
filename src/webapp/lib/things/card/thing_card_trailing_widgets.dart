import 'dart:math';

import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:get/get_connect/http/src/utils/utils.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/things/card/charger/charger_status.dart';
import 'package:iotic/things/card/thing_card_controller.dart';
import 'package:iotic/things/card/thing_gauge.dart';
import 'package:iotic/things/components/multi_channel_switch.dart';
import 'package:iotic/things/data/thing_property.dart';

class ThingCardTrailingWidgets extends StatelessWidget {
  final String _id;
  final bool isPinnedCard;
  final ThingCardController controller;

  final _webSocket = Get.find<WebSocketDataSource>();
  final _focusNode = FocusNode();
  late final TextEditingController _editingController;

  ThingCardTrailingWidgets(
    this._id, {
    super.key,
    required this.isPinnedCard,
    required this.controller,
  });

  @override
  Widget build(BuildContext context) {
    if (isPinnedCard) {
      if (controller.isOn.value != null) {
        return Switch(
          value: controller.isOn.value!,
          onChanged: (value) {
            _webSocket.sendThingPropertyValue(
                _id, ThingPropertyKey.power_control, value);
          },
        );
      } else if (controller.icon.value == Icons.ev_station) {
        return ChargerStatus(_id);
      } else if (controller.icon.value == Icons.local_fire_department) {
        if (controller.voltages.isNotEmpty) {
          RadialGaugeBar bar0 = RadialGaugeBar(
              value: controller.voltages[0],
              color: IoticTheme.orange,
              unit: '%');
          RadialGaugeBar bar1 = RadialGaugeBar(
              value: controller.voltages[1],
              color: IoticTheme.yellow,
              unit: '%');

          return ThingGauge(
            bars: [bar0, bar1],
            min: 0,
            max: 100,
            startAngle: -270,
            endAngle: -45,
          );
        }
      } else if (controller.type == "relay" &&
          controller.multistateSelector.value != null) {
        return MultiChannelSwitch(
            selected: controller.multistateSelector,
            isEnabled: controller.isEnabled,
            onChanged: (selected) {
              _webSocket.sendThingPropertyValue(
                  _id, ThingPropertyKey.multistateSelector, selected);
            });
      } else if (controller.type == "solarInverter" &&
          controller.dcPower.isNotEmpty) {
        RadialGaugeBar bar0 = RadialGaugeBar(
            value: controller.dcPower.first,
            color: IoticTheme.green,
            unit: 'W');
        RadialGaugeBar bar1 = RadialGaugeBar(
            value: controller.dcPower.last, color: IoticTheme.blue, unit: 'W');

        return Obx(() => ThingGauge(
              bars: [bar0, bar1],
              min: 0,
              max: max(
                  max(controller.dcPower.first.value,
                      controller.dcPower.last.value),
                  1000),
              startAngle: -270,
              endAngle: -45,
            ));
      } else {
        return Icon(controller.icon.value);
      }
    }

    return Wrap(children: [
      (controller.isEditingMode.value
          ? IconButton(
              icon: const Icon(Icons.save_outlined),
              onPressed: _saveName,
            )
          : IconButton(
              icon: const Icon(Icons.edit_outlined),
              onPressed: () {
                controller.isEditingMode.value = true;
                _editingController.text = controller.name.value;
                _focusNode.requestFocus();
              },
            )),
      (controller.isPinned.value
          ? IconButton(
              icon: const Icon(Icons.bookmark_added),
              onPressed: () {
                _webSocket.sendThingPropertyValue(
                    _id, ThingPropertyKey.pinned, false);
              },
            )
          : IconButton(
              icon: const Icon(Icons.bookmark_add_outlined),
              onPressed: () {
                _webSocket.sendThingPropertyValue(
                    _id, ThingPropertyKey.pinned, true);
              },
            ))
    ]);
  }

  void _saveName() {
    _webSocket.sendThingPropertyValue(
        _id, ThingPropertyKey.name, _editingController.text);
    controller.isEditingMode.value = false;
  }
}
