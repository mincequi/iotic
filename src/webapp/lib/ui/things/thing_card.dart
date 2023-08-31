import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/ui/things/thing_card_controller.dart';
import '../../data/repository.dart';

class ThingCard extends StatelessWidget {
  late final _control = Get.put(ThingCardController(_id), tag: _id);

  ThingCard(this._id, {super.key});

  final String _id;

  final _repo = Get.find<Repository>();

  @override
  Widget build(BuildContext context) {
    return Column(children: [
      Card(
          child: Obx(
        () => ListTile(
            leading: Icon(_control.icon.value, size: 32),
            title: Text(_control.name.value),
            subtitle: _control.propertyWidgets.isNotEmpty
                ? Row(children: _control.propertyWidgets.values.toList())
                : null,
            trailing: _control.hasPowerControl.value
                ? Switch(
                    activeColor: Colors.yellow,
                    value: _control.powerControl.value,
                    onChanged: (value) {
                      _repo.set(_id, WritableThingProperty.powerControl,
                          value ? 1.0 : 0.0);
                    },
                  )
                : null),
      ))
    ]);
  }
}
