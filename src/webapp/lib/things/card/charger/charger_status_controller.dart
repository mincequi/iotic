import 'dart:async';

import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/things/card/thing_card_controller.dart';
import 'package:iotic/things/data/thing_properties.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/common/web_socket_data_source.dart';

enum ChargerPhase { off, switchingOn, on, switchingOff }

class ChargerStatusController extends GetxController {
  ChargerStatusController(this._id);

  final String _id;
  final WebSocketDataSource _repo = Get.find<WebSocketDataSource>();
  late final _thingControl = Get.find<ThingCardController>(tag: _id);

  // Off -> OnePhase  // PhaseOneSwitchingOn
  // Off -> ThreePhase // PhaseOneSwitchingOn && // PhaseTwoThreeeSwitchingOn
  // OnePhase -> Off  // PhaseOneSwitchingOff
  // ThreePhase -> Off  // PhaseOneSwitchingOff && // PhaseTwoThreeeSwitchingOff
  // OnePhase -> ThreePhase // PhaseOneOn && PhaseTwoThreeSwitchingOn
  // ThreePhase -> OnePhase // PhaseOneOn && PhaseTwoThreeSwitchingOff
  final isPhaseOneOn = false.obs;
  final phaseOneDuration = 0.obs;
  final isPhaseTwoThreeOn = false.obs;
  final phaseTwoDuration = 0.obs;
  final phaseThreeDuration = 0.obs;
  final countdown = 0.obs;
  final _backendCountdown = 0.obs;
  final countdownText = ''.obs;

  final _isTimedOn = false.obs;
  var _phasesCount = 0;
  var _nextPhasesCount = 0;
  var _isPluggedIn = false;

  @override
  void onReady() {
    _assignProperties(_repo.things);

    Timer.periodic(const Duration(milliseconds: 1000), (timer) {
      _isTimedOn.toggle();
      if (countdown.value > 0) {
        countdown.value--;
      }
      // off -> singlePhase (Y)
      // off -> threePhase (Y)
      // singlePhase -> off (Y)
      // singlePhase -> threePhase (Y)
      // threePhase -> off (Y)
      // threePhase -> singlePhase (Y)
      _update(_phasesCount, _nextPhasesCount);
    });

    _repo.things.listen((p0) {
      _assignProperties(p0);
    });
    _thingControl.status.listen((p0) {
      _isPluggedIn = p0 == Icons.flash_on_sharp;
    });
    _backendCountdown.listen((p0) {
      countdown.value = p0;
    });
    countdown.listen((p0) {
      countdownText.value = formatDuration(p0);
    });

    super.onReady();
  }

  void _assignProperties(Map<String, ThingProperties> props) {
    var p0 = props[_id]?.properties;
    if (p0 == null) return;

    _phasesCount = p0[ThingPropertyKey.phases] ?? _phasesCount;
    _nextPhasesCount = p0[ThingPropertyKey.next_phases] ?? _nextPhasesCount;
    _backendCountdown.value =
        p0[ThingPropertyKey.countdown] ?? _backendCountdown.value;
  }

  void _update(int curr, int next) {
    ChargerPhase phaseOne = ChargerPhase.off;
    ChargerPhase phaseTwoThree = ChargerPhase.off;

    if (!_isPluggedIn) {
      curr = 0;
      next = 0;
    }

    if (curr == next) {
      phaseOne = curr != 0 ? ChargerPhase.on : ChargerPhase.off;
      phaseTwoThree = curr == 3 ? ChargerPhase.on : ChargerPhase.off;
    }

    if (curr == 0) {
      if (next == 1) {
        // off -> singlePhase
        phaseOne = ChargerPhase.switchingOn;
      } else if (next == 3) {
        // off -> threePhase
        phaseOne = ChargerPhase.switchingOn;
        phaseTwoThree = ChargerPhase.switchingOn;
      }
    } else if (curr == 1) {
      if (next == 0) {
        // singlePhase -> off
        phaseOne = ChargerPhase.switchingOff;
      } else if (next == 3) {
        // singlePhase -> threePhase
        phaseOne = ChargerPhase.on;
        phaseTwoThree = ChargerPhase.switchingOn;
      }
    } else if (curr == 3) {
      if (next == 0) {
        // threePhase -> off
        phaseOne = ChargerPhase.switchingOff;
        phaseTwoThree = ChargerPhase.switchingOff;
      } else if (next == 1) {
        // threePhase -> singlePhase
        phaseOne = ChargerPhase.on;
        phaseTwoThree = ChargerPhase.switchingOff;
      }
    }

    isPhaseOneOn.value = (phaseOne == ChargerPhase.on) ||
        ((phaseOne == ChargerPhase.switchingOn ||
                phaseOne == ChargerPhase.switchingOff) &&
            _isTimedOn.value);
    isPhaseTwoThreeOn.value = (phaseTwoThree == ChargerPhase.on) ||
        ((phaseTwoThree == ChargerPhase.switchingOn ||
                phaseTwoThree == ChargerPhase.switchingOff) &&
            _isTimedOn.value);

    if (phaseOne == ChargerPhase.switchingOff) {
      if (_isTimedOn.value) {
        phaseOneDuration.value = 150;
      } else {
        phaseOneDuration.value = 2500;
      }
    } else if (phaseOne == ChargerPhase.switchingOn) {
      if (_isTimedOn.value) {
        phaseOneDuration.value = 1500;
      } else {
        phaseOneDuration.value = 250;
      }
    }

    if (phaseTwoThree == ChargerPhase.switchingOff) {
      if (_isTimedOn.value) {
        phaseTwoDuration.value = 200;
        phaseThreeDuration.value = 250;
      } else {
        phaseTwoDuration.value = 2000;
        phaseThreeDuration.value = 1500;
      }
    } else if (phaseTwoThree == ChargerPhase.switchingOn) {
      if (_isTimedOn.value) {
        phaseTwoDuration.value = 2000;
        phaseThreeDuration.value = 2500;
      } else {
        phaseTwoDuration.value = 200;
        phaseThreeDuration.value = 150;
      }
    }
  }

  String formatDuration(int seconds) {
    int minutes = seconds ~/ 60;
    int remainingSeconds = seconds % 60;
    String minutesStr = (minutes < 10) ? '$minutes' : '$minutes';
    String secondsStr =
        (remainingSeconds < 10) ? '0$remainingSeconds' : '$remainingSeconds';
    return '$minutesStr:$secondsStr ';
  }
}
