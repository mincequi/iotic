import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';

import 'thing_card.dart';

class ThingCardSite extends StatelessWidget {
  const ThingCardSite(this._id, {super.key});

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
