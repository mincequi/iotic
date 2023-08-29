import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:iotic/ui/components/legacy/live/live_model.dart';

class ValueUnitTable extends ConsumerWidget {
  const ValueUnitTable(this.id, {Key? key}) : super(key: key);
  final String id;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final ValueUnitModel powerModel = ref.watch(powerModels(id));
    final ValueUnitModel? energyImportModel = ref.watch(energyImportModels(id));
    final ValueUnitModel? energyExportModel = ref.watch(energyExportModels(id));

    return Table(
      textBaseline: TextBaseline.alphabetic,
      defaultVerticalAlignment: TableCellVerticalAlignment.middle,
      columnWidths: const {
        0: IntrinsicColumnWidth(),
        1: IntrinsicColumnWidth(),
        2: IntrinsicColumnWidth(),
      },
      children: [
        TableRow(
          children: <Widget>[
            const Spacer(),
            Container(
              alignment: Alignment.bottomRight,
              padding: const EdgeInsets.only(right: 4.0),
              child: Text(
                powerModel.value
                    .toStringAsFixed(powerModel.value < 100 ? 1 : 0),
                style:
                    const TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
              ),
            ),
            Align(
              alignment: Alignment.bottomRight,
              child: Text(
                powerModel.unit,
                style:
                    const TextStyle(fontSize: 14, fontWeight: FontWeight.bold),
              ),
            ),
          ],
        ),
        if (energyExportModel != null)
          TableRow(
            children: <Widget>[
              const Align(
                alignment: Alignment.bottomLeft,
                child: Icon(
                  Icons.arrow_right,
                  size: 18.0,
                ),
              ),
              Container(
                alignment: Alignment.bottomRight,
                padding: const EdgeInsets.only(right: 4.0),
                child: const Text(
                  '87.6',
                  style: TextStyle(fontSize: 12, color: Colors.white60),
                ),
              ),
              const Align(
                alignment: Alignment.bottomRight,
                child: Text(
                  'kWh',
                  style: TextStyle(fontSize: 10, color: Colors.white60),
                ),
              ),
            ],
          ),
        if (energyImportModel != null)
          TableRow(
            children: <Widget>[
              const Spacer(),
              Container(
                alignment: Alignment.bottomRight,
                padding: const EdgeInsets.only(right: 4.0),
                child: const Text(
                  '54.2',
                  style: TextStyle(fontSize: 12, color: Colors.white60),
                ),
              ),
              const Text(
                'kWh',
                style: TextStyle(fontSize: 10, color: Colors.white60),
              ),
            ],
          ),
      ],
    );
  }
}
