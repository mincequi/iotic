import 'dart:async';

import 'package:flutter/material.dart';
import 'package:get/get_rx/src/rx_types/rx_types.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/common/web_socket_handler.dart';
import 'package:iotic/logs/log_service.dart';
import 'package:iotic/site/card/data/site_data_historic.dart';
import 'package:iotic/site/card/data/site_data_live.dart';
import 'package:iotic/things/data/rx_properties.dart';
import 'package:iotic/things/data/thing_key.dart';
import 'package:iotic/things/data/thing_properties.dart';
import 'package:iotic/things/data/thing_property.dart';

class PreviewDataSource
    with WidgetsBindingObserver
    implements WebSocketDataSource {
  @override
  void dispose() {
    // TODO: implement dispose
  }

  @override
  // TODO: implement logger
  LogService get logger => throw UnimplementedError();

  @override
  void registerHandler(WebSocketHandler handler) {
    // TODO: implement registerHandler
  }

  @override
  void requestSiteDataHistoric(int from, int to) {
    // TODO: implement requestSiteDataHistoric
  }

  @override
  void sendThingPropertyValue(
      String thingId, ThingPropertyKey property, value) {
    // TODO: implement sendThingPropertyValue
  }

  @override
  // TODO: implement settings
  RxMap<ThingKey, RxProperties> get settings => throw UnimplementedError();

  @override
  // TODO: implement siteDataHistoric
  Rx<SiteDataHistoric> get siteDataHistoric => throw UnimplementedError();

  @override
  // TODO: implement siteDataLive
  Rx<SiteDataLive> get siteDataLive => throw UnimplementedError();

  @override
  // TODO: implement things
  RxMap<String, ThingProperties> things = <String, ThingProperties>{}.obs;

  // Start timer to update the things with random data
  Timer? _timer;
  PreviewDataSource() {
    var things_ = <String, ThingProperties>{};
    for (int i = 0; i < 5; i++) {
      things_["thing_$i"] = _thingProperties(i);
    }
    things.value = things_;
    things.refresh();

    _timer?.cancel();
    _timer = Timer.periodic(const Duration(seconds: 4), (timer) {
      for (int i = 0; i < 5; i++) {
        // Increment energy by 0.1 kWh
        things["thing_$i"]!.properties[ThingPropertyKey.energy] =
            (things["thing_$i"]!.properties[ThingPropertyKey.energy] ?? 0) +
                0.07;
      }
      things.refresh();
    });
  }

  // Timer.periodic(const Duration(seconds: 1), (timer) {

  ThingProperties _thingProperties(int id) {
    ThingProperties thingProperties = ThingProperties();
    thingProperties.properties[ThingPropertyKey.name] = "Thing $id";
    thingProperties.properties[ThingPropertyKey.pinned] = true;
    thingProperties.properties[ThingPropertyKey.type] = "relay";
    thingProperties.properties[ThingPropertyKey.icon] = Icons.device_hub;
    thingProperties.properties[ThingPropertyKey.energy] = id;
    thingProperties.properties[ThingPropertyKey.power] = id;

    return thingProperties;
  }
}
