import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';
import 'package:iotic/common/value_unit.dart';

import 'site_readouts_controller.dart';

class SiteReadouts extends StatelessWidget {
  static const double _cornerRadius = 16.0;

  const SiteReadouts({super.key});

  @override
  Widget build(BuildContext context) {
    final c = Get.put(SiteReadoutsController());

    return Container(
        margin: const EdgeInsets.all(_cornerRadius),
        child: Table(
          defaultVerticalAlignment: TableCellVerticalAlignment.baseline,
          textBaseline: TextBaseline.ideographic,
          columnWidths: const <int, TableColumnWidth>{
            0: IntrinsicColumnWidth(),
            1: FixedColumnWidth(56),
            //2: FixedColumnWidth(24),
          },
          children: [
            TableRow(
              children: [
                Obx(() =>
                    ValueUnit(Icons.solar_power, c.pvPower.value, c.pvColor()))
              ],
            ),
            TableRow(
              children: [
                Obx(() => ValueUnit(Icons.home, c.sitePower.value,
                    Theme.of(context).colorScheme.primary))
              ],
            ),
            TableRow(
              children: [
                Obx(() =>
                    ValueUnit(Icons.power, c.gridPower.value, c.gridColor()))
              ],
            ),
          ],
        ));
  }
}
