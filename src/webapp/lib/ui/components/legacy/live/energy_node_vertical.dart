import 'package:iotic/ui/components/legacy/live/live_model.dart';

import 'value_unit_table.dart';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

class EnergyNodeVertical extends ConsumerWidget {
  const EnergyNodeVertical(this.id, {Key? key}) : super(key: key);
  final String id;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final IconData _iconData = ref.watch(iconProvider(id));

    return Container(
      height: 200,
      width: 72,
      padding: const EdgeInsets.symmetric(vertical: 8.0, horizontal: 8.0),
      //color: Colors.red,
      child: Stack(
        children: [
          Align(
            alignment: Alignment.center,
            child: Icon(
              _iconData,
              size: 56.0,
              color: Theme.of(context).colorScheme.onBackground,
            ),
          ),
          Align(
            alignment: Alignment.centerRight,
            child: Container(
              margin: const EdgeInsets.only(top: 108.0),
              child: ValueUnitTable(id),
            ),
          ),
        ],
      ),
    );
  }
}
