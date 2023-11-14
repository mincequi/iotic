import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';
import 'package:iotic/data/repository.dart';
import 'package:iotic/data/thing_id.dart';
import 'package:iotic/data/thing_property.dart';

class SettingsSlider extends StatelessWidget {
  SettingsSlider(this._sectionName, {super.key});

  final ThingId _sectionName;
  final _repo = Get.find<Repository>();
  late final _properties = _repo.settings[_sectionName]?.properties;
  late final ThingProperty _property = _properties!.keys.first;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(children: [
        ListTile(
          contentPadding: EdgeInsets.only(left: 22.0, right: 24),
          dense: true,
          leading: Text(
            "τ",
            style: TextStyle(fontSize: 30, height: 0.5),
          ),
          title: Text(_property.toUiString()),
          trailing: Obx(() => Text(
                _properties![_property].toString() + " s",
              )),
        ),
        Obx(() => Slider(
              min: 1.0,
              max: 10.0,
              divisions: 9,
              value: _properties?[_property],
              onChanged: (double value) {
                _repo.set(_sectionName.name, _property, value);
              },
            )),
      ]),
    );
  }
}
