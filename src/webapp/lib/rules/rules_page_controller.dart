import 'package:get/get.dart';
import 'package:iotic/rules/data/rule.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/things/data/thing_service.dart';

class RulesPageController extends GetxController {
  final _thingService = Get.find<ThingService>();

  final rules = RxMap<String, Rule>().obs;

  @override
  void onInit() {
    super.onInit();
    _mapThingsToRules();

    _thingService.things.listen((event) {
      _mapThingsToRules();
    });
  }

  void _mapThingsToRules() {
    final List<Rule> tempList = [];
    _thingService.thingsList.forEach((thing) {
      // Check if thing is already in rules
      if (rules.value.containsKey(thing.id)) {
        return;
      }

      // Only add items that have power_control or actuation
      if (!thing.properties.keys.contains(ThingPropertyKey.power_control) &&
          !thing.properties.keys.contains(ThingPropertyKey.actuation)) {
        return;
      }

      rules.value[thing.id] = Rule(thingId: thing.id, name: thing.name.value.isEmpty ? thing.id : thing.name.value);
    });

    //rules.assignAll(tempList);
  }
}
