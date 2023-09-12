import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/ui/things/thing_card_controller.dart';

class ThingSlider extends StatelessWidget {
  late final _control = Get.find<ThingCardController>(tag: _id);

  ThingSlider(this._id, {super.key});

  final String _id;
  final _offsets = [
    "Off",
    //"500 W",
    "400 W",
    "250 W",
    "150 W",
    "0 W",
    "150 W",
    "250 W",
    "400 W",
    //"500 W",
    "600 W",
    //"800 W",
    "1 kW",
    //"1.2 kW",
    "1.5 kW",
    //"2.0 kW",
    "2.5 kW",
    //"3 kW",
    "4 kW",
    //"5 kW",
    "6 kW",
    //"8 kW",
    "10 kW"
  ];

  Color _color(BuildContext context) {
    if (_control.offset.value == 0) {
      return Theme.of(context).colorScheme.outline;
    }
    if (_control.offset.value! < 5) {
      return Theme.of(context).colorScheme.secondary;
    }
    //if (_control.offset.value! < 5) {
    //  return Theme.of(context).colorScheme.primary;
    //}
    return Theme.of(context).colorScheme.tertiary;
  }

  @override
  Widget build(BuildContext context) => Obx(() => _control.offset.value != null
      ? Slider(
          activeColor: _color(context),
          thumbColor: _color(context),
          //thumbColor: Theme.of(context).colorScheme.inversePrimary,
          max: (_offsets.length - 1).toDouble(),
          divisions: _offsets.length - 1,
          value: _control.offset.value!,
          label: _offsets[_control.offset.value!.toInt()],
          onChanged: (value) {
            _control.offset.value = value;
          })
      : Container());
}
