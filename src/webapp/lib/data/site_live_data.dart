class SiteLiveData {
  final int ts;
  final int pvPower;
  final int gridPower;
  final int sitePower;

  SiteLiveData(this.ts, this.pvPower, this.gridPower, this.sitePower);

  SiteLiveData.fromMap(Map<String, dynamic> json)
      : ts = json['timestamp'],
        pvPower = json['pv_power'],
        gridPower = json['grid_power'],
        sitePower = json['site_power'];
}
