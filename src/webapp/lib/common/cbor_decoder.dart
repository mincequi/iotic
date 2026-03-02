import 'dart:typed_data';
import 'package:fl_chart/fl_chart.dart';

List<List<FlSpot>> decodeArchivedData(Uint8List bytes) {
  final arrays = <List<int>>[];
  int offset = 0;

  while (offset < bytes.length) {
    final b = bytes[offset++];
    if ((b >> 5) != 4) throw const FormatException('Expected array');
    int length = _readLength(bytes, b & 0x1F, offset);
    offset += _lengthBytes(b & 0x1F);

    final list = List<int>.filled(length, 0, growable: false);
    for (int i = 0; i < length; i++) {
      final val = _readInt(bytes, offset);
      list[i] = val.value;
      offset = val.offset;
    }

    // delta decode in-place
    for (int i = 1; i < list.length; i++) {
      list[i] += list[i - 1];
    }

    arrays.add(list);
  }

  if (arrays.isEmpty) return [];

  // build FlSpot series
  final ts = arrays.first;
  final len = ts.length;
  return [
    for (int s = 1; s < arrays.length; s++)
      List<FlSpot>.generate(
        len,
        (i) => FlSpot((ts[i] - ts[0]).toDouble(), arrays[s][i].toDouble()),
        growable: false,
      )
  ];
}

/// Reads CBOR length (for arrays)
int _readLength(Uint8List bytes, int additional, int offset) {
  if (additional < 24) return additional;
  int readBytes = [1, 2, 4, 8][additional - 24];
  int value = 0;
  for (int i = 0; i < readBytes; i++) {
    value = (value << 8) | bytes[offset + i];
  }
  return value;
}

int _lengthBytes(int additional) {
  if (additional < 24) return 0;
  if (additional <= 27) return [1, 2, 4, 8][additional - 24];
  throw const FormatException('Unsupported length encoding');
}

/// Result of reading a CBOR integer
class _IntResult {
  final int value;
  final int offset;
  _IntResult(this.value, this.offset);
}

/// Reads one CBOR integer starting at offset
_IntResult _readInt(Uint8List bytes, int offset) {
  final b = bytes[offset++];
  final majorType = b >> 5;
  final additional = b & 0x1F;

  int value;
  int lengthBytes = 0;

  if (additional < 24) {
    value = additional;
  } else if (additional <= 27) {
    lengthBytes = [1, 2, 4, 8][additional - 24];
    value = 0;
    for (int i = 0; i < lengthBytes; i++) {
      value = (value << 8) | bytes[offset++];
    }
  } else {
    throw const FormatException('Unsupported integer encoding');
  }

  if (majorType == 0) return _IntResult(value, offset);
  if (majorType == 1) return _IntResult(-1 - value, offset);

  throw FormatException('Expected integer at offset ${offset - 1}');
}
