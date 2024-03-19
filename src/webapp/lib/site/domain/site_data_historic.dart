import 'package:iotic/things/domain/thing_property.dart';

class SiteDataHistoric {
  List<dynamic> ts = List.empty(growable: true);
  List<dynamic> pvPower = List.empty(growable: true);
  List<dynamic> gridPower = List.empty(growable: true);

  SiteDataHistoric(this.ts, this.pvPower, this.gridPower);
  SiteDataHistoric.empty();

  SiteDataHistoric.fromMap(Map<String, dynamic> json)
      : ts = json[ThingProperty.timestamp.name],
        pvPower = json[ThingProperty.pv_power.name],
        gridPower = json[ThingProperty.grid_power.name];
}
