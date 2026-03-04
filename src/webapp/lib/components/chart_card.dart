import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/components/chart_controller.dart';
import 'package:iotic/components/dropdown.dart';
import 'package:iotic/components/chart_selection_controller.dart';

class ChartCard extends StatelessWidget {
  ChartCard({super.key});

  final controller = Get.put(ChartController());
  ChartSelectionController get selectionController => Get.put(ChartSelectionController(controller));

  @override
  Widget build(BuildContext context) {
    return Column(children: [
      SizedBox(
        height: 194,
        width: double.infinity,
        child: Card(
            clipBehavior: Clip.hardEdge,
            child: Stack(children: [
              Positioned.fill(
                  child: GetBuilder<ChartController>(
                      builder: (_) =>
                          LineChart(controller.chartData.value, duration: const Duration(milliseconds: 300)))),
            ])),
        //],
      ),
      Row(
          crossAxisAlignment: CrossAxisAlignment.start,
          //mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Expanded(
                child: Obx(() => Dropdown(
                      label: "Thing",
                      items: selectionController.things,
                      selected: selectionController.selectedThing.value,
                      onChanged: (id) {
                        selectionController.selectedThing.value =
                            selectionController.things.firstWhere((item) => item.id == id);
                      },
                    ))),
            Expanded(
                child: Column(children: [
              Obx(() => Dropdown(
                    label: "Property",
                    items: selectionController.properties,
                    selected: selectionController.selectedProperty.value,
                    onChanged: (id) {
                      selectionController.selectedProperty.value =
                          selectionController.properties.firstWhere((item) => item.id == id);
                    },
                  )),
              Row(
                mainAxisAlignment: MainAxisAlignment.end,
                children: [
                  Obx(() => Text("${selectionController.selectedDate.value.toLocal()}".split(' ')[0],
                      style: const TextStyle(color: Colors.white))),
                  IconButton(
                      onPressed: () {
                        selectionController.stepDate(-1);
                      },
                      icon: const Icon(Icons.chevron_left)),
                  IconButton(
                      onPressed: () {
                        selectionController.stepDate(1);
                      },
                      icon: const Icon(Icons.chevron_right)),
                ],
              ),
            ])),
            const SizedBox(width: 4)
          ]),
    ]);
  }
}
