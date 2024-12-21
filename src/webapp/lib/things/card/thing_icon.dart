import 'package:flutter/material.dart';
import 'package:icon_decoration/icon_decoration.dart';
import 'package:iotic/common/iotic_theme.dart';

class ThingIcon extends StatelessWidget {
  final IconData _iconData;

  const ThingIcon(this._iconData, {super.key});

  @override
  Widget build(BuildContext context) {
    return DecoratedIcon(
      icon: Icon(
        _iconData,
        size: 40,
        color: Colors.transparent,
      ),
      decoration: const IconDecoration(
          border: IconBorder(
        width: IoticTheme.lineWidth,
        color: IoticTheme.other,
      )),
    );
  }
}
