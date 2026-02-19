import 'package:flutter/material.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/things/card/energy_property.dart';
import 'package:iotic/things/card/thing_card_controller.dart';

import 'thing_ui_property.dart';

class ThingCardSubtitle extends StatelessWidget {
  final ThingCardController controller;

  const ThingCardSubtitle({required this.controller, Key? key})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    dynamic p;
    final propertyWidgets = <StatelessWidget>[];

    if (controller.dcEnergy.isNotEmpty) {
      for (var i = 0; i < controller.dcEnergy.length; i++) {
        propertyWidgets.add(EnergyProperty(
          Icons.electric_bolt,
          controller.dcEnergy[i],
          'kWh',
          //color: i == 0 ? IoticTheme.green : IoticTheme.blue,
        ));
      }
    }

    if (controller.energy != null) {
      propertyWidgets
          .add(EnergyProperty(Icons.electric_bolt, controller.energy!, 'kWh'));
      //color: IoticTheme.yellow));
    }
    if (controller.power.value != null) {
      propertyWidgets.add(ThingUiProperty(
        Icons.electric_bolt,
        controller.power,
        "W",
      ));
    }
    if ((p = controller.temperature.value) != null) {
      propertyWidgets.add(ThingUiProperty(
        Icons.thermostat,
        controller.temperature,
        '°C',
        factor: 0.1,
      ));
    }

    return Row(
      children: propertyWidgets,
      spacing: 12.0,
    );
  }
}
