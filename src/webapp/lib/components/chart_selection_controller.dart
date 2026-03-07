import 'package:get/get.dart';
import 'package:iotic/components/chart_controller.dart';
import 'package:iotic/components/dropdown.dart';
import 'package:iotic/things/data/thing_property.dart';
import 'package:iotic/things/data/thing_service.dart';

class ChartSelectionController extends GetxController {
  final ThingService _thingService = Get.find();
  final ChartController chartController;

  ChartSelectionController(this.chartController);

  // Only include things that have properties power or dcPower
  late final List<DropdownItem> things = _thingService.things.entries
      .map((entry) {
        if (!(entry.value.properties.containsKey(ThingPropertyKey.power) ||
            entry.value.properties.containsKey(ThingPropertyKey.dcPower))) {
          return null;
        }
        return DropdownItem(
          id: entry.key,
          name: entry.value.properties[ThingPropertyKey.name]?.toString() ?? entry.key,
        );
      })
      .whereType<DropdownItem>()
      .toList();

  final selectedThing = Rx<DropdownItem?>(null);

  final properties = <DropdownItem>[].obs;

  final selectedProperty = Rx<DropdownItem?>(null);

  final selectedDate = Rx<DateTime>(DateTime.now().subtract(const Duration(days: 1)));

  void stepDate(int days) {
    selectedDate.value = selectedDate.value.add(Duration(days: days));
  }

  // Combine your three selections into one Rx
  final combinedSelection = <String, String>{}.obs;

  @override
  void onReady() {
    super.onReady();

    everAll([selectedThing, selectedProperty, selectedDate], (_) {
      final thing = selectedThing.value;
      final property = selectedProperty.value;
      final date = selectedDate.value;

      if (thing != null && property != null) {
        combinedSelection.value = {
          'thingId': thing.id,
          'propertyId': property.id.split('.').last,
          'date': date.toIso8601String(),
        };
      }
    });

    // Debounce the combined Rx
    debounce<Map<String, String>>(
      combinedSelection,
      (value) {
        chartController.loadData(
          thingId: value['thingId']!,
          propertyId: value['propertyId']!,
          date: selectedDate.value,
        );
      },
      time: const Duration(milliseconds: 100),
    );

    ever(selectedThing, (DropdownItem? thing) {
      if (thing == null) return;

      final thingData = _thingService.things[thing.id];
      if (thingData == null) return;

      final newProperties = thingData.properties.entries
          .where((entry) => entry.key == ThingPropertyKey.power || entry.key == ThingPropertyKey.dcPower)
          .map((entry) => DropdownItem(
                id: entry.key.name,
                name: entry.key.name,
              ))
          .toList();

      properties.assignAll(newProperties);

      if (newProperties.isNotEmpty) {
        selectedProperty.value ??= newProperties.first;

        // If selectedProperty is not in the new properties list, reset it to first property
        if (!newProperties.any((p) => p.id == selectedProperty.value?.id)) {
          selectedProperty.value = newProperties.first;
        }
      }
    });

    if (things.isNotEmpty) {
      selectedThing.value = things.first;
    }
  }
}
