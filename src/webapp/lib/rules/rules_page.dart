import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:iotic/rules/rule_card.dart';
import 'package:iotic/rules/rules_page_controller.dart';

class RulesPage extends StatelessWidget {
  RulesPage({super.key});

  final _control = Get.put(RulesPageController());

  @override
  Widget build(BuildContext context) {
    return Obx(() => ListView.builder(
        itemCount: _control.rules.value.length,
        itemBuilder: (context, index) {
          return RuleCard(
              _control.rules.value.entries.toList()[index].value.getName());
        }));
  }
}
