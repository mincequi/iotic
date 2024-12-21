import 'package:flutter/material.dart';
import 'package:get/get_core/get_core.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/get_state_manager.dart';
import 'package:icon_decoration/icon_decoration.dart';
import 'package:iotic/things/card/charger/charger_status_controller.dart';
import 'package:iotic/common/iotic_theme.dart';

class ChargerStatus extends StatelessWidget {
  ChargerStatus(this._id, {Key? key}) : super(key: key);

  final String _id;
  late final _control = Get.put(ChargerStatusController(_id), tag: _id);

  @override
  Widget build(BuildContext context) => Obx(() => Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.end,
          children: [
            Stack(clipBehavior: Clip.none, children: [
              const Positioned(
                  child: DecoratedIcon(
                      icon: Icon(
                        Icons.flash_on_sharp,
                        color: Colors.black,
                      ),
                      decoration: IconDecoration(
                          border: IconBorder(
                              width: IoticTheme.lineWidth * 2,
                              color: IoticTheme.other)))),
              Positioned(
                  child: AnimatedOpacity(
                      opacity: _control.isPhaseTwoThreeOn.value ? 1.0 : 0.0,
                      duration: Duration(
                          milliseconds: _control.phaseThreeDuration.value),
                      child: const DecoratedIcon(
                          icon: Icon(
                            Icons.flash_on_sharp,
                          ),
                          decoration: IconDecoration(
                              border: IconBorder(
                            width: IoticTheme.lineWidth * 2,
                            color: Colors.black,
                          ))))),
              const Positioned(
                  right: 6,
                  child: DecoratedIcon(
                      icon: Icon(
                        Icons.flash_on_sharp,
                        color: Colors.black,
                      ),
                      decoration: IconDecoration(
                          border: IconBorder(
                        width: IoticTheme.lineWidth * 2,
                        color: IoticTheme.other,
                      )))),
              Positioned(
                  right: 6,
                  child: AnimatedOpacity(
                      opacity: _control.isPhaseTwoThreeOn.value ? 1.0 : 0.0,
                      duration: Duration(
                          milliseconds: _control.phaseTwoDuration.value),
                      child: const DecoratedIcon(
                          icon: Icon(
                            Icons.flash_on_sharp,
                          ),
                          decoration: IconDecoration(
                              border: IconBorder(
                            width: IoticTheme.lineWidth * 2,
                            color: Colors.black,
                          ))))),
              const Positioned(
                  right: 12,
                  child: DecoratedIcon(
                      icon: Icon(
                        Icons.flash_on_sharp,
                        color: Colors.black,
                      ),
                      decoration: IconDecoration(
                          border: IconBorder(
                        width: IoticTheme.lineWidth * 2,
                        color: IoticTheme.other,
                      )))),
              Positioned(
                  right: 12,
                  child: AnimatedOpacity(
                      opacity: _control.isPhaseOneOn.value ? 1.0 : 0.0,
                      duration: Duration(
                          milliseconds:
                              _control.phaseOneDuration.value), // 1500 // 800
                      child: const DecoratedIcon(
                          icon: Icon(
                            Icons.flash_on_sharp,
                          ),
                          decoration: IconDecoration(
                              border: IconBorder(
                            width: IoticTheme.lineWidth * 2,
                            color: Colors.black,
                          ))))),
            ]),
            Visibility(
                visible: _control.countdown.value > 0,
                child: Obx((() => Text(_control.countdownText.value,
                    style: const TextStyle(fontSize: 10)))))
          ]));

  String formatDuration(int seconds) {
    int minutes = seconds ~/ 60;
    int remainingSeconds = seconds % 60;
    String minutesStr = (minutes < 10) ? '$minutes' : '$minutes';
    String secondsStr =
        (remainingSeconds < 10) ? '0$remainingSeconds' : '$remainingSeconds';
    return '$minutesStr:$secondsStr';
  }
}
