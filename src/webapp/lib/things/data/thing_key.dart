// ignore_for_file: constant_identifier_names

enum ThingKey {
  // custom properties - things
  site,
  ev_charging_strategy
}

extension ThingKeyFormatting on ThingKey {
  String toUiString() {
    switch (this) {
      case ThingKey.site:
        return "Site";
      case ThingKey.ev_charging_strategy:
        return "EV Charging Strategy";
    }
  }
}
