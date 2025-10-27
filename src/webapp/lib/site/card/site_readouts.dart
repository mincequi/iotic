import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/get_instance.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';
import 'package:iotic/common/text_util.dart';
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
            1: IntrinsicColumnWidth(),
            2: FixedColumnWidth(20),
          },
          children: [
            TableRow(
              children: [
                Obx(() =>
                    ValueUnit(Icons.solar_power, c.pvPower.value, c.pvColor())),
                const SizedBox(width: 4),
                Obx(() => Text(powerUnit(c.pvPower.value),
                    style: textStyle(c.pvColor().withAlpha(200))))
              ],
            ),
            TableRow(
              children: [
                Obx(() => ValueUnit(Icons.home, c.sitePower.value,
                    Theme.of(context).colorScheme.primary)),
                const SizedBox(width: 4),
                Obx(() => Text(powerUnit(c.sitePower.value),
                    style: textStyle(
                        Theme.of(context).colorScheme.primary.withAlpha(200))))
              ],
            ),
            TableRow(
              children: [
                Obx(() =>
                    ValueUnit(Icons.power, c.gridPower.value, c.gridColor())),
                const SizedBox(width: 4),
                Obx(() => Text(powerUnit(c.gridPower.value),
                    style: textStyle(c.gridColor().withAlpha(200))))
              ],
            ),
          ],
        ));
  }
}
