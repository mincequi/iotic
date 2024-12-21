import 'package:flutter/material.dart';
import 'package:iotic/things/data/thing_key.dart';
import 'package:iotic/settings/settings_slider.dart';

class SettingsCard extends StatelessWidget {
  const SettingsCard(this._sectionName, {super.key});

  final ThingKey _sectionName;

  @override
  Widget build(BuildContext context) {
    return Container(
      child: Column(children: [
        Container(
          child: Text(_sectionName.toUiString()),
          alignment: Alignment.bottomLeft,
          padding: const EdgeInsets.only(left: 12),
        ),
        SettingsSlider(_sectionName),
      ]),
    );
  }
}
