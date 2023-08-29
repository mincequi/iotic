import '../components/legacy/live/energy_flow.dart';
import '../components/legacy/live/energy_node_horizontal.dart';
import '../components/legacy/live/energy_node_vertical.dart';
import 'package:flutter/material.dart';

class Live extends StatelessWidget {
  const Live({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Align(
      alignment: Alignment.topCenter,
      child: SizedBox(
        width: 288,
        height: 248,
        child: Container(
          margin: const EdgeInsets.all(16.0),
          child: const Stack(
            //mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Align(
                alignment: Alignment.topCenter,
                child: EnergyNodeHorizontal('generator'),
              ),
              Align(
                alignment: Alignment.bottomCenter,
                child: EnergyNodeHorizontal('home'),
              ),
              Align(
                alignment: Alignment.centerLeft,
                child: EnergyNodeVertical('grid'),
              ),
              Align(
                alignment: Alignment.center,
                child: SizedBox(
                  width: 112,
                  height: 72,
                  child: EnergyFlow(),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
