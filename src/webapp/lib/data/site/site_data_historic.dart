import 'package:iotic/data/things/thing_property.dart';

class SiteDataHistoric {
  List<dynamic> ts = List.empty(growable: true);
  List<dynamic> pvPower = List.empty(growable: true);
  List<dynamic> gridPower = List.empty(growable: true);

  SiteDataHistoric(this.ts, this.pvPower, this.gridPower);
  SiteDataHistoric.empty();

  SiteDataHistoric.fromMap(Map<String, dynamic> json)
      : ts = json[ThingPropertyKey.timestamp.name],
        pvPower = json[ThingPropertyKey.pv_power.name],
        gridPower = json[ThingPropertyKey.grid_power.name];
}
