import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:icon_decoration/icon_decoration.dart';
import 'package:iotic/themes/iotic_theme.dart';
import '../../common/data/web_socket_data_source.dart';
import '../domain/thing_property.dart';
import 'thing_card_controller.dart';
import 'thing_slider.dart';

class ThingCard extends StatelessWidget {
  ThingCard(this._id, {this.isPinnedCard = false, super.key}) {
    _editingController = TextEditingController(text: _control.name.value);
  }

  final String _id;
  late final _control = Get.put(ThingCardController(_id), tag: _id);

  final bool isPinnedCard;

  final _repo = Get.find<WebSocketDataSource>();
  final _focusNode = FocusNode();
  late final TextEditingController _editingController;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Obx(
        () => Column(children: [
          ListTile(
            dense: true,
            leading: DecoratedIcon(
              icon: Icon(
                _control.icon.value,
                size: 40,
                color: Colors.transparent,
              ),
              decoration: const IconDecoration(
                  border: IconBorder(
                width: 1.5,
                color: IoticTheme.other,
              )),
            ),
            title: titleWidget(),
            //Text(_control.name.value),
            subtitle: _control.propertyWidgets.isNotEmpty
                ? Row(children: _control.propertyWidgets.values.toList())
                : null,
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
              value: _control.isOn.value!,
              onChanged: (value) {
                _repo.sendThingPropertyValue(
                    _id, ThingProperty.power_control, value);
              },
            )
          : Icon(_control.status.value);
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
                _repo.sendThingPropertyValue(_id, ThingProperty.pinned, false);
              },
            )
          : IconButton(
              icon: const Icon(Icons.bookmark_add_outlined),
              onPressed: () {
                _repo.sendThingPropertyValue(_id, ThingProperty.pinned, true);
              },
            ))
    ]);
  }

  void _saveName() {
    _repo.sendThingPropertyValue(
        _id, ThingProperty.name, _editingController.text);
    _control.isEditingMode.value = false;
  }
}
