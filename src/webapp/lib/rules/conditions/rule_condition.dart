import 'package:iotic/things/data/thing_property.dart';

class RuleCondition {
  String thingId;
  ThingPropertyKey thingProperty;
  dynamic thingVale;
  int operator = 0;

  static const operators = {
    '==',
    '!=',
    '>',
    '<',
    '>=',
    '<=',
  };

  RuleCondition(this.thingId, this.thingProperty, this.thingVale);
}
