import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/rules/conditions/condition_controller.dart';
import 'package:iotic/rules/conditions/condition_dropdown.dart';
import 'package:iotic/rules/conditions/condition_dropdown2.dart';
import 'package:iotic/things/data/thing.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/things/data/thing_service.dart';

final thingsService = Get.find<ThingService>();

class RuleConditionContainer extends StatelessWidget {
  RuleConditionContainer(this._id, this._key) {
    switch (_key) {
      case ThingPropertyKey.on_condition:
        _backgroundText = "ON";
        _backgroundColor = IoticTheme.green;
        break;
      case ThingPropertyKey.on_condition:
        _backgroundText = "OFF";
        _backgroundColor = IoticTheme.orange;
        break;
    }
  }

  final String _id;
  final ThingPropertyKey _key;
  late Color _backgroundColor;
  String _backgroundText = "";
  late final _control =
      Get.put(RuleConditionController(_id, _key), tag: _id + _backgroundText);
  final _selectedThing = ValueNotifier<Thing?>(null);

  @override
  Widget build(BuildContext context) {
    return Container(
        margin: const EdgeInsets.only(left: 10, right: 10, bottom: 10),
        padding: const EdgeInsets.only(
            left: IoticTheme.rectangleRadius,
            right: IoticTheme.rectangleRadius),
        //top: IoticTheme.regularSpace),
        //bottom: IoticTheme.regularSpace),
        height: 22,
        decoration: BoxDecoration(
          borderRadius: const BorderRadius.all(
            Radius.circular(IoticTheme.rectangleRadius),
          ),
          gradient: LinearGradient(
              colors: [
                _backgroundColor.withOpacity(0.4),
                _backgroundColor.withOpacity(0.2)
              ],
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
              stops: const [
                0.0,
                1.0,
              ],
              tileMode: TileMode.clamp),
        ),
        child: Stack(children: [
          Align(
            alignment: Alignment.bottomRight,
            child: Container(
                padding: const EdgeInsets.only(top: 2),
                child: Text(_backgroundText,
                    textAlign: TextAlign.right,
                    style: const TextStyle(
                        letterSpacing: -2.0,
                        fontSize: 16,
                        fontWeight: FontWeight.w900,
                        color: Colors.black))),
          ),
          Row(children: [
            //DropdownMenu(dropdownMenuEntries: dropdownMenuEntries)
            GenericDropdown(
                label: 'Select a thing',
                items: thingsService.thingsList,
                selectedItem: _control.selectedThing,
                onSelected: _control.selectThing,
                itemLabelBuilder: (Thing thing) => thing.name.value,
                itemHeight: 16.0,
                dropdownHeight: 100.0,
                backgroundColor: _backgroundColor),
            /*ConditionDropdown<Thing>(
                items: thingsService.things.values,
                itemToString: (Thing t) => t.name.value,
                valueListenable: _control.selectedThing,
                backgroundColor: _backgroundColor),*/
            Container(
              margin: const EdgeInsets.all(IoticTheme.rectangleRadius),
              width: 4,
              height: 4,
              decoration: BoxDecoration(
                color: _backgroundColor,
                shape: BoxShape.circle,
                boxShadow: const [
                  BoxShadow(
                    color: Colors.black,
                    offset: Offset(
                        IoticTheme.shadowOffset, IoticTheme.shadowOffset),
                    blurRadius: 0,
                  ),
                ],
              ),
            ),
            Obx(() => ConditionDropdown<ThingPropertyKey>(
                items: _control.availableProperties.toList(),
                itemToString: (ThingPropertyKey t) =>
                    t.toString().split('.').last,
                valueListenable: _control.selectedProperty,
                backgroundColor: _backgroundColor)),
          ]),
        ]));
  }
}
