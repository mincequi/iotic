import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/common/iotic_theme.dart';

import '../../common/web_socket_data_source.dart';
import '../data/thing_property.dart';
import 'thing_card_controller.dart';
import 'thing_slider_thumb.dart';
import 'thing_slider_track.dart';

class ThingSlider extends StatelessWidget {
  late final _control = Get.find<ThingCardController>(tag: _id);
  //late final _control = Get.put(ThingCardController(_id));
  final _repo = Get.find<WebSocketDataSource>();

  ThingSlider(this._id, {super.key});

  final String _id;
  final _offsets = [
    "Off",
    "500 W",
    //"300 W",
    "200 W",
    "0 W",
    "200 W",
    //"300 W",
    "500 W",
    //"700 W",
    "1 kW",
    //"1.5 kW",
    "2 kW",
    //"3 kW",
    "5 kW",
    //"7 kW",
    "10 kW",
    "Max"
  ];

  Color _color(BuildContext context) {
    if (_control.offset.value == 0) {
      return Theme.of(context).colorScheme.outline;
    }
    if (_control.offset.value! < 3) {
      return Theme.of(context).colorScheme.secondary;
    }
    if (_control.offset.value == 3) {
      return Theme.of(context).colorScheme.primary;
    }
    //if (_control.offset.value! < 5) {
    //  return Theme.of(context).colorScheme.primary;
    //}
    return Theme.of(context).colorScheme.tertiary;
  }

  @override
  Widget build(BuildContext context) => Obx(() => _control.offset.value != null
      ? Container(
          margin: const EdgeInsets.only(left: 12, right: 12, bottom: 12.0),
          padding: const EdgeInsets.only(left: 3.0, right: 3.0),
          height: 22,
          decoration: BoxDecoration(
            borderRadius: const BorderRadius.all(
              Radius.circular(IoticTheme.rectangleRadius),
            ),
            gradient: LinearGradient(
                colors: [
                  IoticTheme.other.withOpacity(_opacity),
                  Theme.of(context).colorScheme.secondary.withOpacity(_opacity),
                  Theme.of(context).colorScheme.secondary.withOpacity(_opacity),
                  Theme.of(context).colorScheme.primary.withOpacity(_opacity),
                  Theme.of(context).colorScheme.primary.withOpacity(_opacity),
                  Theme.of(context).colorScheme.tertiary.withOpacity(_opacity),
                ],
                begin: Alignment.centerLeft,
                end: Alignment.centerRight,
                stops: [
                  0.5 / (_offsets.length - 1),
                  (1.5) / (_offsets.length - 1),
                  (2.0) / (_offsets.length - 1),
                  (3.0) / (_offsets.length - 1),
                  (3.5) / (_offsets.length - 1),
                  //(6) / (_offsets.length - 1),
                  (_offsets.length / 2) / (_offsets.length - 1),
                ],
                tileMode: TileMode.clamp),
          ),
          child: Stack(children: [
            Align(
              alignment: Alignment.centerRight,
              child: Container(
                  padding: const EdgeInsets.only(right: 0, top: 5),
                  child: const Text("PV OFFSET",
                      textAlign: TextAlign.right,
                      style: TextStyle(
                          letterSpacing: -2.0,
                          fontSize: 16,
                          fontWeight: FontWeight.w900,
                          color: Colors.black))),
            ),
            SliderTheme(
                data: SliderTheme.of(context).copyWith(
                    trackHeight: 0.0,
                    trackShape: ThingSliderTrack(),
                    //activeTickMarkColor: Colors.transparent,
                    //inactiveTickMarkColor: Colors.transparent,
                    inactiveTrackColor: Colors.transparent,
                    activeTrackColor: Colors.transparent,
                    thumbShape: ThingSliderThumb(
                      thumbRadius: 8.0,
                      offsets: _offsets,
                      color: Theme.of(context).colorScheme.onPrimary,
                    )),
                child: Slider(
                    activeColor: _color(context),
                    thumbColor: _color(context),
                    //thumbColor: Theme.of(context).colorScheme.inversePrimary,
                    max: (_offsets.length - 1).toDouble(),
                    divisions: _offsets.length - 1,
                    value: _control.offset.value!.toDouble(),
                    //label: _offsets[_control.offset.value!.toInt()],
                    onChanged: (value) {
                      _control.offset.value = value.round();
                      _repo.sendThingPropertyValue(
                          _id, ThingPropertyKey.offset, value);
                    }))
          ]))
      : Container());

  final double _opacity = 0.4;
}
