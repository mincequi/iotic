import 'package:collection/collection.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:test/test.dart';

void add(List<FlSpot> oldList, List<FlSpot> newList) {
  final i = lowerBound(oldList, newList.first, compare: (FlSpot s, FlSpot t) {
    if (s.x < t.x)
      return -1;
    else if (s.x > t.x)
      return 1;
    else
      return 0;
  });

  final j = lowerBound(oldList, newList.last, compare: (FlSpot s, FlSpot t) {
    if (s.x <= t.x)
      return -1;
    else if (s.x > t.x)
      return 1;
    else
      return 0;
  });

  oldList.removeRange(i, j);
  oldList.insertAll(i, newList);
}

void main() {
  test('Counter value should be incremented', () {
    var oldList = List<FlSpot>.from([
      FlSpot(20, 10), // requested 2
      FlSpot(40, 10), // requested 1
      FlSpot(60, 10), // old
      FlSpot(80, 10), // old
      FlSpot(100, 10), // old
    ]);

    var emptyList = List<FlSpot>.empty(growable: true);

    final newList = List<FlSpot>.from([
      FlSpot(40, 20), // new
      FlSpot(60, 20), // new
    ]);

    add(oldList, newList);
    add(emptyList, newList);

    print(oldList);
    print(emptyList);
  });
}
