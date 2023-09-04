import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/ui/things/thing_card_controller.dart';
import '../../data/repository.dart';

class ThingCard extends StatelessWidget {
  late final _control = Get.put(ThingCardController(_id), tag: _id);

  ThingCard(this._id, {this.isOnSite = false, super.key}) {
    _editingController = TextEditingController(text: _control.name.value);
  }

  final String _id;
  final bool isOnSite;
  late final TextEditingController _editingController;

  final _repo = Get.find<Repository>();

  final _focusNode = FocusNode();

  @override
  Widget build(BuildContext context) {
    return Card(
        child: Obx(
      () => /*Column(children: [*/
          ListTile(
        leading: Icon(_control.icon.value, size: 32),
        title: titleWidget(),
        //Text(_control.name.value),
        subtitle: _control.propertyWidgets.isNotEmpty
            ? Row(children: _control.propertyWidgets.values.toList())
            : null,
        //isThreeLine: _control.propertyWidgets.isNotEmpty,
        trailing: trailingButtons(),
      ),
    ));
  }

  Widget titleWidget() {
    if (_control.isEditingMode.value) {
      return TextField(
        controller: _editingController,
        decoration: null,
        focusNode: _focusNode,
        onSubmitted: (value) {
          _saveName();
        },
      );
    } else {
      return Obx(() => Text(_control.name.value));
    }
  }

  Widget? trailingButtons() {
    if (isOnSite) {
      return _control.hasPowerControl.value
          ? Switch(
              activeColor: Colors.yellow,
              value: _control.powerControl.value,
              onChanged: (value) {
                _repo.set(_id, WritableThingProperty.powerControl, value);
              },
            )
          : null;
    }

    return Wrap(children: [
      (_control.isEditingMode.value
          ? IconButton(
              icon: const Icon(Icons.save_outlined),
              onPressed: _saveName,
            )
          : IconButton(
              icon: const Icon(Icons.edit_outlined),
              onPressed: () {
                _control.isEditingMode.value = true;
                _focusNode.requestFocus();
              },
            )),
      (_control.isOnSite.value
          ? IconButton(
              icon: const Icon(Icons.bookmark_added),
              onPressed: () {
                _repo.set(_id, WritableThingProperty.isOnSite, false);
              },
            )
          : IconButton(
              icon: const Icon(Icons.bookmark_add_outlined),
              onPressed: () {
                _repo.set(_id, WritableThingProperty.isOnSite, true);
              },
            ))
    ]);
  }

  void _saveName() {
    _repo.set(_id, WritableThingProperty.name, _editingController.text);
    _control.isEditingMode.value = false;
  }
}
