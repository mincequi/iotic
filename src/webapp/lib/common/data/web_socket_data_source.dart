// ignore_for_file: avoid_web_libraries_in_flutter

import 'dart:html' as html;
import 'dart:convert';
import 'dart:typed_data';
import 'package:cbor/simple.dart';
import 'package:flutter/foundation.dart';
import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/things/domain/rx_properties.dart';
import 'package:iotic/things/domain/thing_id.dart';
import 'package:iotic/things/domain/thing_properties.dart';
import 'package:iotic/things/domain/thing_property.dart';
import 'package:iotic/site/domain/site_data_historic.dart';
import 'package:iotic/site/domain/site_data_live.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class WebSocketDataSource {
  final siteDataLive = SiteDataLive(0, 0, 0, 0).obs;
  final siteDataHistoric = SiteDataHistoric.empty().obs;
  final things = <String, ThingProperties>{}.obs;
  final settings = RxMap<ThingId, RxProperties>();

  WebSocketDataSource() {
    html.document.addEventListener("visibilitychange", _onVisibilityChange);
    _connect(false);
  }

  void sendThingPropertyValue(
      String thingId, ThingProperty property, dynamic value) {
    _channel.sink.add(jsonEncode({
      thingId: {property.name: value}
    }));
  }

  void requestSiteDataHistoric(int from, int to) {
    _channel.sink.add(Uint8List.fromList(cbor.encode({
      ThingId.site.name: {
        ThingProperty.timestamp.name: [from, to]
      }
    })));
  }

  //final _host = html.window.location.hostname;
  //final _port = int.parse(html.window.location.port);
  final _host = "localhost";
  final _port = 8030;

  late WebSocketChannel _channel;

  void _connect(bool doDisconnect) {
    if (doDisconnect) {
      _channel.sink.close();
    }
    _channel = WebSocketChannel.connect(Uri.parse('ws://$_host:$_port/ws'));

    _channel.stream.forEach((element) {
      if (element.runtimeType == Uint8List) return _parseBinary(element);

      // JSON always has string as keys
      Map<String, dynamic> json = jsonDecode(element);
      // Only accept jsons with exactly one top level object
      if (json.length != 1) return;

      var entry = json.entries.first;

      // 1. Check if this is site
      if (_parseSite(entry)) return;

      // 2. Check if this is EV Charging Strategy
      if (_parseSettings(entry)) return;

      _parseThing(entry);
    });
  }

  void _parseBinary(Uint8List message) {
    final obj = cbor.decode(message);
    //if (obj.runtimeType != Map<String, dynamic>) return;
    final map = (obj as Map);
    if (!map.containsKey(ThingId.site.name)) return;
    final values = map[ThingId.site.name] as Map;
    // Timestamp
    final timestamps = (values[ThingProperty.timestamp.name] as List);
    for (int i = 0; i < timestamps.length - 1; i++) {
      timestamps[i + 1] += timestamps[i];
    }
    // PvPower
    final pvPowers = (values[ThingProperty.pv_power.name] as List);
    pvPowers.first *= 10;
    for (int i = 0; i < pvPowers.length - 1; i++) {
      pvPowers[i + 1] = pvPowers[i] + pvPowers[i + 1] * 10;
    }
    // GridPower
    final gridPowers = (values[ThingProperty.grid_power.name] as List);
    gridPowers.first *= 10;
    for (int i = 0; i < gridPowers.length - 1; i++) {
      gridPowers[i + 1] = gridPowers[i] + gridPowers[i + 1] * 10;
    }
    siteDataHistoric.value = SiteDataHistoric(timestamps, pvPowers, gridPowers);
  }

  bool _parseSite(MapEntry<String, dynamic> entry) {
    if (entry.key != "site") return false;

    var map = Map<String, dynamic>.from(entry.value);
    if (map.values.first is List) {
      siteDataHistoric.value = SiteDataHistoric.fromMap(map);
      siteDataLive.value = SiteDataLive(
          siteDataHistoric.value.ts.last,
          siteDataHistoric.value.pvPower.last,
          siteDataHistoric.value.gridPower.last,
          -siteDataHistoric.value.pvPower.last -
              siteDataHistoric.value.gridPower.last);
      return true;
    }

    var liveData = SiteDataLive.fromMap(map);
    if (liveData != null) {
      siteDataLive.value = liveData;
      return true;
    }

    if (!settings.containsKey(ThingId.site)) {
      settings[ThingId.site] = RxProperties.fromMap(entry.value);
    } else {
      ThingProperties.fromMap(entry.value).properties.forEach((key, value) {
        settings[ThingId.site]?.properties[key] = value;
      });
    }

    return true;
  }

  bool _parseSettings(MapEntry<String, dynamic> entry) {
    if (entry.key != "ev_charging_strategy") return false;

    ThingId thingId = ThingId.ev_charging_strategy;

    if (!settings.containsKey(thingId)) {
      settings[thingId] = RxProperties.fromMap(entry.value);
    } else {
      ThingProperties.fromMap(entry.value).properties.forEach((key, value) {
        settings[thingId]?.properties[key] = value;
      });
    }

    return true;
  }

  void _parseThing(MapEntry<String, dynamic> entry) {
    if (!things.containsKey(entry.key)) {
      things[entry.key] = ThingProperties.fromMap(entry.value);
      return;
    }
    ThingProperties.fromMap(entry.value).properties.forEach((key, value) {
      things[entry.key]?.properties[key] = value;
    });
    // TODO: is this needed?
    things.refresh();
  }

  void _onVisibilityChange(html.Event e) {
    if (html.document.hidden ?? false) return;

    _connect(true);
  }
}
