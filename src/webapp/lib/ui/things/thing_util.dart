import 'package:flutter/material.dart';

IconData? toIcon(dynamic v) {
  switch (v) {
    case 0:
      return null;
    case 1:
      return Icons.power_off_sharp;
    case 2:
      return Icons.mode_night_sharp;
    case 3:
      return Icons.timelapse_sharp;
    case 4:
      return Icons.battery_3_bar_sharp;
    //case 5: return Icons.turt
    case 6:
      return Icons.battery_charging_full_sharp;
    case 7:
      return Icons.error_sharp;
    case 8:
      return Icons.medical_services_sharp;
    default:
      return null;

    // unknown         = 0, //invalid
    // off             = 1,
    // idle            = 2, //sleeping
    // waiting         = 3, //starting
    // charging        = 4, //mpp//on//active
    // throttled       = 5,
    // complete        = 6, //shuttingDown
    // error           = 7,
    // service         = 8
  }
}
