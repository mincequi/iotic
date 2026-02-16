import 'package:flutter/material.dart';
import 'package:get/get.dart';

class MultiChannelSwitch extends StatelessWidget {
  final Rxn<List<bool>> selected;
  final RxBool isEnabled;
  final Function(List<bool>) onChanged;

  const MultiChannelSwitch(
      {super.key,
      required this.selected,
      required this.isEnabled,
      required this.onChanged});

  @override
  Widget build(BuildContext context) {
    return Obx(() => SegmentedButton<int>(
          multiSelectionEnabled: true,
          emptySelectionAllowed: true,
          showSelectedIcon: false,
          segments: List.generate(selected.value?.length ?? 0, (index) {
            return ButtonSegment<int>(
              value: index,
              //label: Text(index.toString()),
              icon: const Icon(
                Icons.flash_on_sharp,
              ),
              //enabled: enabledStates[index],
            );
          }),
          selected: {
            for (int i = 0; i < (selected.value?.length ?? 0); i++)
              if (selected.value![i]) i,
          },
          onSelectionChanged: isEnabled.value
              ? (value) {
                  final List<bool> selected = List<bool>.generate(
                    this.selected.value?.length ?? 0,
                    (index) => value.contains(index),
                  );
                  onChanged(selected);
                }
              : null,
          style: const ButtonStyle(
            visualDensity: VisualDensity(
              horizontal: -4.0,
              vertical: -2,
            ),
          ),
        ));
  }
}
