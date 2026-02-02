import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/things/card/charger/charger_status.dart';
import 'package:iotic/things/card/thing_card_controller.dart';
import 'package:iotic/things/card/thing_gauge.dart';
import 'package:iotic/things/data/thing_property.dart';

class TrailingWidgets extends StatelessWidget {
  final String _id;
  final bool isPinnedCard;
  final ThingCardController control;

  final _repo = Get.find<WebSocketDataSource>();
  final _focusNode = FocusNode();
  late final TextEditingController _editingController;

  TrailingWidgets(
    this._id, {
    super.key,
    required this.isPinnedCard,
    required this.control,
  });

  @override
  Widget build(BuildContext context) {
    if (isPinnedCard) {
      if (control.isOn.value != null) {
        return Switch(
          value: control.isOn.value!,
          onChanged: (value) {
            _repo.sendThingPropertyValue(
                _id, ThingPropertyKey.power_control, value);
          },
        );
      } else if (control.icon.value == Icons.ev_station) {
        return ChargerStatus(_id);
      } else if (control.icon.value == Icons.local_fire_department) {
        dynamic p;
        if ((p = control.voltages.value) != null) {
          // Round to multiple of 5
          double p0 = (p[0] / 5).round() * 5;
          double p1 = (p[1] / 5).round() * 5;

          RadialGaugeBar bar0 =
              RadialGaugeBar(value: p0, color: IoticTheme.orange);
          RadialGaugeBar bar1 =
              RadialGaugeBar(value: p1, color: IoticTheme.yellow);

          return Row(mainAxisSize: MainAxisSize.min, spacing: 0, children: [
            ThingGauge(
              bars: [bar0],
              min: 0,
              max: 100,
              startAngle: -270,
              endAngle: -45,
            ),
            ThingGauge(
              bars: [bar1],
              min: 0,
              max: 100,
              startAngle: -270,
              endAngle: -45,
            )
          ]);
        }
      } else if (control.type == "relay" &&
          control.multistateSelector.value != null) {
        final length = control.multistateSelector.value?.length;
        return SegmentedButton<int>(
          multiSelectionEnabled: true,
          emptySelectionAllowed: true,
          showSelectedIcon: false,
          segments: List.generate(length!, (index) {
            return ButtonSegment<int>(
              value: index,
              //label: Text(index.toString()),
              icon: const Icon(
                Icons.flash_on_sharp,
              ),
              //enabled: enabledStates[index],
            );
          }),
          selected: {
            for (int i = 0; i < length; i++)
              if (control.multistateSelector.value![i]) i,
          },
          onSelectionChanged: (value) {
            final List<bool> selected = List<bool>.generate(
              length,
              (index) => value.contains(index),
            );
            _repo.sendThingPropertyValue(
                _id, ThingPropertyKey.multistateSelector, selected);
          },
        );
      } else {
        return Icon(control.icon.value);
      }
    }

    return Wrap(children: [
      (control.isEditingMode.value
          ? IconButton(
              icon: const Icon(Icons.save_outlined),
              onPressed: _saveName,
            )
          : IconButton(
              icon: const Icon(Icons.edit_outlined),
              onPressed: () {
                control.isEditingMode.value = true;
                _editingController.text = control.name.value;
                _focusNode.requestFocus();
              },
            )),
      (control.isPinned.value
          ? IconButton(
              icon: const Icon(Icons.bookmark_added),
              onPressed: () {
                _repo.sendThingPropertyValue(
                    _id, ThingPropertyKey.pinned, false);
              },
            )
          : IconButton(
              icon: const Icon(Icons.bookmark_add_outlined),
              onPressed: () {
                _repo.sendThingPropertyValue(
                    _id, ThingPropertyKey.pinned, true);
              },
            ))
    ]);
  }

  void _saveName() {
    _repo.sendThingPropertyValue(
        _id, ThingPropertyKey.name, _editingController.text);
    control.isEditingMode.value = false;
  }
}
