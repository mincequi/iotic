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
    if ((p = controller.power.value) != null) {
      propertyWidgets
          .add(ThingUiProperty(Icons.electric_bolt, p, _powerUnit(p.round())));
    }
    if ((p = controller.temperature.value) != null) {
      propertyWidgets.add(ThingUiProperty(Icons.thermostat, p / 10.0, '°C'));
    }

    return Row(
      children: propertyWidgets,
      spacing: 12.0,
    );
  }

  static String _powerUnit(int v) {
    if (v.abs() < 1000) return "W";
    return "kW";
  }
}
