import 'package:flutter/material.dart';
import 'package:iotic/ui/pages/value_unit.dart';
import 'package:iotic/ui/things/thing_property.dart';
import '../../data/thing_live_data.dart';

class ThingCard extends StatelessWidget {
  ThingCard(this._name, this._properties, {super.key}) {
    _name = _properties[ReadableThingProperty.name] ?? _name;
    var type = _properties[ReadableThingProperty.type];
    _icon = _typeToIcon[type] ?? Icons.device_hub;
    if (_properties.containsKey(ReadableThingProperty.powerControl)) {
      _hasPowerControl = true;
      _powerControl = _properties[ReadableThingProperty.powerControl] != 0;
      if (_icon == Icons.device_hub) {
        _icon = Icons.electrical_services;
      }
    }

    dynamic p;
    if ((p = _properties[ReadableThingProperty.power]) != null) {
      _propertyWidgets
          .add(ThingProperty(Icons.electric_bolt, p, powerUnit(p.round())));
    }
    if ((p = _properties[ReadableThingProperty.temperature]) != null) {
      _propertyWidgets.add(ThingProperty(Icons.thermostat, p, '°C'));
    }
  }

  String _name;
  Map<ReadableThingProperty, dynamic> _properties;
  List<ThingProperty> _propertyWidgets = List.empty(growable: true);
  IconData _icon = Icons.device_hub;
  bool _hasPowerControl = false;
  bool _powerControl = false;

  @override
  Widget build(BuildContext context) {
    return Column(children: [
      Card(
        child: ListTile(
            leading: Icon(_icon, size: 32),
            title: Text(_name),
            subtitle: _propertyWidgets.isNotEmpty
                ? Row(
                    //mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: _propertyWidgets)
                : null,
            trailing: _hasPowerControl
                ? Switch(
                    activeColor: Colors.yellow,
                    value: _powerControl,
                    onChanged: (value) {},
                  )
                : null),
      )
    ]);
  }

  String powerUnit(int v) {
    if (v.abs() < 1000) return "W";
    return "kW";
  }

  static const Map<String?, IconData> _typeToIcon = {
    "smartMeter": Icons.electric_meter,
    "solarInverter": Icons.solar_power,
    "evStation": Icons.ev_station,
    null: Icons.device_hub
  };
}
