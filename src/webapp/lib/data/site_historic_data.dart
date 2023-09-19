class SiteHistoricData {
  List<dynamic> ts = List.empty(growable: true);
  List<dynamic> pvPower = List.empty(growable: true);
  List<dynamic> gridPower = List.empty(growable: true);
  List<dynamic> sitePower = List.empty(growable: true);

  SiteHistoricData();

  SiteHistoricData.fromMap(Map<String, dynamic> json)
      : ts = json['timestamp'],
        pvPower = json['pv_power'],
        gridPower = json['grid_power'],
        sitePower = json['site_power'];
}
