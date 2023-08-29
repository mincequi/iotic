import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

final iconProvider = Provider.family<IconData, String>((ref, id) {
  final _icons = {
    'generator': Icons.wb_sunny,
    'grid': Icons.flash_on,
    'home': Icons.home,
  };
  return _icons[id] != null ? _icons[id]! : Icons.error;
});

final powerModels =
    ChangeNotifierProvider.family<ValueUnitModel, String>((ref, id) {
  final _powerModels = {
    'generator': ValueUnitModel(value: 898.0, unit: 'W'),
    'grid': ValueUnitModel(value: 209.0, unit: 'kW'),
    'home': ValueUnitModel(value: 3.0, unit: 'kW'),
  };
  return _powerModels[id] != null ? _powerModels[id]! : ValueUnitModel();
});

final energyImportModels =
    ChangeNotifierProvider.family<ValueUnitModel?, String>((ref, id) {
  final _energyModels = {
    'grid': ValueUnitModel(value: 87.1, unit: 'kWh'),
    'home': ValueUnitModel(value: 23.6, unit: 'kWh'),
  };
  return _energyModels[id];
});

final energyExportModels =
    ChangeNotifierProvider.family<ValueUnitModel?, String>((ref, id) {
  final _energyModels = {
    'generator': ValueUnitModel(value: 145.9, unit: 'kWh'),
    'grid': ValueUnitModel(value: 75.3, unit: 'kWh'),
  };
  return _energyModels[id];
});

class ValueUnitModel with ChangeNotifier {
  ValueUnitModel({this.value = 0.0, this.unit = ''});

  double value = 0.0;
  var unit = '';
}
