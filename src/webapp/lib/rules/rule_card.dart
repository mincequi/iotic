import 'package:flutter/material.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/rules/conditions/condition_container.dart';
import 'package:iotic/things/card/thing_icon.dart';
import 'package:iotic/things/data/thing_property.dart';

class RuleCard extends StatelessWidget {
  final String _thindId;

  const RuleCard(this._thindId, {Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Card(
        child: Column(children: [
      ListTile(
        dense: true,
        leading: const ThingIcon(Icons.electrical_services_sharp),
        title: Text(_thindId),
        //subtitle: Text(rule.description),
        //trailing: Icon(Icons.arrow_forward),
      ),
      RuleConditionContainer(_thindId, ThingPropertyKey.on_condition),
      RuleConditionContainer(_thindId, ThingPropertyKey.off_condition),
    ]));
  }
}
