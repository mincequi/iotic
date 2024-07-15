import 'package:iotic/rules/conditions/rule_condition.dart';

class Rule {
  final String thingId;
  final String? name;
  final RuleCondition? onCondition;
  final RuleCondition? offCondition;

  String getName() =>
      name ?? thingId; // If name is null, return thingId as name

  Rule({
    required this.thingId,
    this.name,
    this.onCondition,
    this.offCondition,
  });
}
