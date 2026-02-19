import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/things/components/odometer.dart';

class EnergyProperty extends StatelessWidget {
  const EnergyProperty(this._icon, this._value, this._unit,
      {this.color, super.key});

  final IconData _icon;
  final RxDouble _value;
  final String _unit;
  final Color? color;

  @override
  Widget build(BuildContext context) {
    return Card(
        margin: const EdgeInsets.all(0),
        child:
            Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
          const SizedBox(
            width: 4,
          ),
          Icon(
            size: 14,
            _icon,
            color: color,
          ),
          //const SizedBox(width: 3),
          SizedBox(
              width: 36,
              child: Align(
                  alignment: Alignment.centerRight,
                  child: Obx(() => Odometer(
                        value: _value.value.abs(),
                        digits: 2,
                        style: Theme.of(context)
                            .textTheme
                            .bodyMedium
                            ?.copyWith(color: color),
                      )))),
          const SizedBox(width: 2),
          Text(
            _unit,
            style:
                Theme.of(context).textTheme.bodySmall?.copyWith(color: color),
            //textAlign: TextAlign.left,
          ),
          const SizedBox(
            width: 4,
          )
        ]));
  }
}
