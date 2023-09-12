import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:iotic/ui/things/thing_card.dart';

class SiteThingCard extends StatelessWidget {
  const SiteThingCard(this._id, {super.key});

  @override
  Widget build(BuildContext context) {
    return Column(children: [
      ThingCard(
        _id,
        isPinnedCard: true,
      ),
      //Slider(min: -2, max: 10, value: 0, onChanged: (value) {}),
    ]);
  }

  final String _id;
}
