import 'package:flutter/material.dart';
import 'package:iotic/things/card/energy_property.dart';
import 'package:iotic/things/card/thing_card_controller.dart';

import 'thing_ui_property.dart';

class ThingCardSubtitle extends StatelessWidget {
  final ThingCardController controller;

  ThingCardSubtitle({required this.controller, Key? key}) : super(key: key) {
    energyNotifier.value = (controller.energy.value ?? 0) / 10;
  }

  final ValueNotifier<double> energyNotifier = ValueNotifier(0);

  @override
  Widget build(BuildContext context) {
    dynamic p;
    final propertyWidgets = <StatelessWidget>[];

    if ((p = controller.energy.value) != null) {
      propertyWidgets.add(EnergyProperty(Icons.electric_bolt, p, 'kWh'));
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
