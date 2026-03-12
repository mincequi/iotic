import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/foundation.dart';
import 'package:http/http.dart' as http;
import 'package:iotic/io/cbor_decoder.dart';

class HttpService {
  HttpService() {
    if (kDebugMode) {
      _host = "raspberrypi.local";
      //_host = "localhost";
      _port = 8030;
    } else {
      _host = Uri.base.host;
      _port = Uri.base.port;
    }
  }

  late String _host;
  late int _port;
  final _httpClient = http.Client();

  Future<List<List<FlSpot>>> fetchArchivedData(String thingId, String propertyId, DateTime date) async {
    final uri = _buildUri(thingId, propertyId, date);
    final bytes = await _fetchBytes(uri);
    var data = decodeArchivedData(bytes);
    return data;
  }

  Uri _buildUri(String thingId, String propertyId, DateTime date) {
    final y = date.year.toString().padLeft(4, '0');
    final m = date.month.toString().padLeft(2, '0');
    final d = date.day.toString().padLeft(2, '0');

    final formatted = '$y-$m-$d';

    return Uri.parse('http://$_host:$_port/database/$thingId/$propertyId/$formatted');
  }

  Future<Uint8List> _fetchBytes(Uri uri) async {
    final response = await _httpClient.get(uri);

    if (response.statusCode != 200) {
      throw Exception('HTTP ${response.statusCode}');
    }

    return response.bodyBytes;
  }
}
