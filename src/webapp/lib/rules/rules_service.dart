import 'package:iotic/rules/data/rule.dart';

class RulesService {
  static Future<List<Rule>> fetchRules() async {
    await Future.delayed(Duration(seconds: 1));
    return List.generate(10, (index) => Rule(thingId: 'Rule $index'));
  }
}
