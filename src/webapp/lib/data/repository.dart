// ignore_for_file: avoid_web_libraries_in_flutter, iterable_contains_unrelated_type

import 'dart:html' as html;
import 'dart:convert';
import 'package:get/get_rx/get_rx.dart';
import 'package:iotic/data/rx_properties.dart';
import 'package:iotic/data/site_historic_data.dart';
import 'package:iotic/data/site_live_data.dart';
import 'package:iotic/data/thing_id.dart';
import 'package:iotic/data/properties.dart';
import 'package:iotic/data/thing_property.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class Repository /*extends FullLifeCycleController with FullLifeCycleMixin*/ {
  final siteLiveData = SiteLiveData(0, 0, 0, 0).obs;
  final siteHistoricData = SiteHistoricData().obs;
  final things = <String, Properties>{}.obs;
  final settings = RxMap<ThingId, RxProperties>();

  void set(String id, ThingProperty property, dynamic value) {
    var json = jsonEncode({
      id: {property.name: value}
    });
    _channel.sink.add(json);
  }

  final _host = html.window.location.hostname;
  final _port = int.parse(html.window.location.port);
  //final _host = "raspberrypi";
  //final _port = 8030;

  late WebSocketChannel _channel;

  Repository() {
    html.document.addEventListener("visibilitychange", onVisibilityChange);
    connect(false);
  }

  void connect(bool doDisconnect) {
    if (doDisconnect) {
      _channel.sink.close();
    }
    _channel = WebSocketChannel.connect(Uri.parse('ws://$_host:$_port/ws'));

    _channel.stream.forEach((element) {
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

  bool _parseSite(MapEntry<String, dynamic> entry) {
    if (entry.key != "site") return false;

    var map = Map<String, dynamic>.from(entry.value);
    if (map.values.first is List) {
      siteHistoricData.value = SiteHistoricData.fromMap(map);
      siteLiveData.value = SiteLiveData(
          siteHistoricData.value.ts.last,
          siteHistoricData.value.pvPower.last,
          siteHistoricData.value.gridPower.last,
          siteHistoricData.value.sitePower.last);
      return true;
    }

    var liveData = SiteLiveData.fromMap(map);
    if (liveData != null) {
      siteLiveData.value = liveData;
      return true;
    }

    if (!settings.containsKey(ThingId.site)) {
      settings[ThingId.site] = RxProperties.fromMap(entry.value);
    } else {
      Properties.fromMap(entry.value).properties.forEach((key, value) {
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
      Properties.fromMap(entry.value).properties.forEach((key, value) {
        settings[thingId]?.properties[key] = value;
      });
    }

    return true;
  }

  void _parseThing(MapEntry<String, dynamic> entry) {
    if (!things.containsKey(entry.key)) {
      things[entry.key] = Properties.fromMap(entry.value);
      return;
    }
    Properties.fromMap(entry.value).properties.forEach((key, value) {
      things[entry.key]?.properties[key] = value;
    });
    // TODO: is this needed?
    things.refresh();
  }

  void onVisibilityChange(html.Event e) {
    if (html.document.hidden ?? false) return;

    connect(true);
    //set("ui", WritableThingProperty.isVisible, true);
    // do something
  }

  /*
  @override
  void onResumed() {
    connect();
  }

  @override
  void onDetached() {
    // TODO: implement onDetached
  }

  @override
  void onInactive() {
    // TODO: implement onInactive
  }

  @override
  void onPaused() {
    // TODO: implement onPaused
  }
  */
}
