import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/components/chart_controller.dart';
import 'package:iotic/components/dropdown.dart';
import 'package:iotic/components/chart_selection_controller.dart';

class ChartCard extends StatelessWidget {
  ChartCard({super.key});

  final controller = Get.put(ChartController());
  ChartSelectionController get selectionController =>
      Get.put(ChartSelectionController(controller));

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        SizedBox(
            height: 194,
            child: Card(
                clipBehavior: Clip.hardEdge,
                child: Column(children: [
                  Row(
                      //mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Expanded(
                            child: Obx(() => Dropdown(
                                  label: "Thing",
                                  items: selectionController.things,
                                  selected:
                                      selectionController.selectedThing.value,
                                  onChanged: (id) {
                                    selectionController.selectedThing.value =
                                        selectionController.things.firstWhere(
                                            (item) => item.id == id);
                                    //controller.loadDataForThing(id);
                                  },
                                ))),
                        Expanded(
                            child: Obx(() => Dropdown(
                                  label: "Property",
                                  items: selectionController.properties,
                                  selected: selectionController
                                      .selectedProperty.value,
                                  onChanged: (id) {
                                    selectionController.selectedProperty.value =
                                        selectionController.properties
                                            .firstWhere(
                                                (item) => item.id == id);
                                    //controller.loadDataForThing(id);
                                  },
                                ))),
                      ]),
                  Expanded(
                      child: Obx(() => LineChart(controller.chartData.value,
                          duration: const Duration(milliseconds: 500)))),
                ]))),
      ],
    );
  }
}
