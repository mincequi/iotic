// ignore_for_file: constant_identifier_names

enum ThingId {
  // custom properties - things
  site,
  ev_charging_strategy
}

extension ThingIdFormatting on ThingId {
  String toUiString() {
    switch (this) {
      case ThingId.site:
        return "Site";
      case ThingId.ev_charging_strategy:
        return "EV Charging Strategy";
    }
  }
}
