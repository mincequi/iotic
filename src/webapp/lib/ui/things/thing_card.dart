import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/ui/things/thing_card_controller.dart';
import 'package:iotic/ui/things/thing_slider.dart';
import '../../data/repository.dart';
import '../../data/thing_property.dart';

class ThingCard extends StatelessWidget {
  late final _control = Get.put(ThingCardController(_id), tag: _id);

  ThingCard(this._id, {this.isPinnedCard = false, super.key}) {
    _editingController = TextEditingController(text: _control.name.value);
  }

  final String _id;
  final bool isPinnedCard;

  final _repo = Get.find<Repository>();
  final _focusNode = FocusNode();
  late final TextEditingController _editingController;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Obx(
        () => Column(children: [
          ListTile(
            dense: true,
            //visualDensity: const VisualDensity(horizontal: 0, vertical: -2),
            /*minVerticalPadding:
                _control.propertyWidgets.isNotEmpty ? 0.0 : 18.0,*/
            leading: Icon(
              _control.icon.value,
              size: 30,
            ),
            title: titleWidget(),
            //Text(_control.name.value),
            subtitle: _control.propertyWidgets.isNotEmpty
                ? Row(children: _control.propertyWidgets.values.toList())
                : null,
            //isThreeLine: _control.propertyWidgets.isNotEmpty,
            trailing: trailingButtons(context),
          ),
          isPinnedCard ? ThingSlider(_id) : Container(),
        ]),
      ),
    );
  }

  Widget titleWidget() {
    if (_control.isEditingMode.value) {
      return TextField(
        controller: _editingController,
        focusNode: _focusNode,
        onSubmitted: (value) {
          _saveName();
        },
      );
    } else {
      return Obx(() => Text(_control.name.value));
    }
  }

  Widget? trailingButtons(BuildContext context) {
    if (isPinnedCard) {
      return _control.isOn.value != null
          ? Switch(
              trackOutlineWidth:
                  MaterialStateProperty.resolveWith((states) => 2.0),
              value: _control.isOn.value!,
              onChanged: (value) {
                _repo.set(_id, WritableThingProperty.power_control, value);
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
                _editingController.text = _control.name.value;
                _focusNode.requestFocus();
              },
            )),
      (_control.isPinned.value
          ? IconButton(
              icon: const Icon(Icons.bookmark_added),
              onPressed: () {
                _repo.set(_id, WritableThingProperty.pinned, false);
              },
            )
          : IconButton(
              icon: const Icon(Icons.bookmark_add_outlined),
              onPressed: () {
                _repo.set(_id, WritableThingProperty.pinned, true);
              },
            ))
    ]);
  }

  void _saveName() {
    _repo.set(_id, WritableThingProperty.name, _editingController.text);
    _control.isEditingMode.value = false;
  }
}
