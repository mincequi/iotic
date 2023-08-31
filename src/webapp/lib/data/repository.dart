// ignore: avoid_web_libraries_in_flutter
import 'dart:html' as html;
import 'dart:convert';
import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/data/site_live_data.dart';
import 'package:iotic/data/thing_live_data.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

enum WritableThingProperty { powerControl }

class Repository {
  final siteLiveData = SiteLiveData(0, 0, 0).obs;
  final things = <String, ThingLiveData>{}.obs;

  void set(String id, WritableThingProperty property, dynamic value) {
    var json = jsonEncode({
      id.toString(): {property.name: value}
    });
    _channel.sink.add(json);
  }

  //final _port = (int.parse(html.window.location.port) + 1);
  final _port = 7091;

  late WebSocketChannel _channel;

  Repository() {
    _channel = WebSocketChannel.connect(
        Uri.parse('ws://${html.window.location.hostname}:$_port/ws'));
    _channel.stream.forEach((element) {
      Map<String, dynamic> liveData = jsonDecode(element);
      // Only accept jsons with exactly one top level object
      if (liveData.length != 1) return;
      var entry = liveData.entries.first;
      if (entry.key == "site") {
        siteLiveData.value = SiteLiveData.fromMap(entry.value);
        return;
      }
      things[entry.key] = ThingLiveData.fromMap(entry.value);
    });
  }
}
