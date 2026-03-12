import 'package:flutter/material.dart';

import 'package:iotic/components/chart_card.dart';

class HistoryPage extends StatelessWidget {
  const HistoryPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        ChartCard(),
      ],
    );
  }
}
