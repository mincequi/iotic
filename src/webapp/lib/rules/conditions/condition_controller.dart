import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/things/data/thing.dart';
import 'package:iotic/things/data/thing_property.dart';

final allowedProperties = [
  ThingPropertyKey.temperature,
  ThingPropertyKey.power,
  ThingPropertyKey.grid_power,
  ThingPropertyKey.pv_power
];

class RuleConditionController extends GetxController {
  final String _id;
  final ThingPropertyKey _key;
  RuleConditionController(this._id, this._key) {
    ever(selectedThing, (_) {
      if (selectedThing.value == null) {
        availableProperties.clear();
        return;
      }

      availableProperties.assignAll(selectedThing.value!.properties.keys
          .where((element) => allowedProperties.contains(element)));
    });
  }

  final RxString _property = RxString('');
  final RxString _operator = RxString('');
  final RxString _value = RxString('');

  String get property => _property.value;
  String get operator => _operator.value;
  String get value => _value.value;

  final selectedThing = Rxn<Thing>();

  final availableProperties = <ThingPropertyKey>[].obs;
  final selectedProperty = ValueNotifier<ThingPropertyKey?>(null);

  void setProperty(String property) {
    _property.value = property;
  }

  void setOperator(String operator) {
    _operator.value = operator;
  }

  void setValue(String value) {
    _value.value = value;
  }

  void selectThing(Thing? thing) {
    selectedThing.value = thing;
  }
}
