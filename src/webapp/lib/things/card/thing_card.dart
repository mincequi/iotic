import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:icon_decoration/icon_decoration.dart';
import 'package:iotic/common/iotic_theme.dart';
import 'package:iotic/things/card/thing_card_trailing_widgets.dart';
import '../../common/web_socket_data_source.dart';
import '../data/thing_property.dart';
import 'thing_card_controller.dart';
import 'thing_card_subtitle.dart';
import 'thing_slider.dart';

class ThingCard extends StatelessWidget {
  ThingCard(this._id, {this.isPinnedCard = false, super.key}) {
    _editingController = TextEditingController(text: _controller.name.value);
  }

  final String _id;
  late final _controller = Get.put(ThingCardController(_id), tag: _id);

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
              _controller.icon.value,
              size: 40,
              color: Colors.transparent,
            ),
            decoration: const IconDecoration(
                border: IconBorder(
              width: IoticTheme.lineWidth,
              color: IoticTheme.other,
            )),
          ),
          title: titleWidget(),
          //Text(_control.name.value),
          subtitle: _controller.hasSubtitle.value
              ? ThingCardSubtitle(controller: _controller)
              : null,
          trailing: ThingCardTrailingWidgets(_id,
              isPinnedCard: isPinnedCard, controller: _controller),
        ),
        isPinnedCard ? ThingSlider(_id) : Container(),
      ]),
    ));
  }

  Widget titleWidget() {
    if (_controller.isEditingMode.value) {
      return TextField(
        controller: _editingController,
        focusNode: _focusNode,
        onSubmitted: (value) {
          _saveName();
        },
      );
    } else {
      return Obx(() => Text(_controller.name.value));
    }
  }

  void _saveName() {
    _repo.sendThingPropertyValue(
        _id, ThingPropertyKey.name, _editingController.text);
    _controller.isEditingMode.value = false;
  }
}
