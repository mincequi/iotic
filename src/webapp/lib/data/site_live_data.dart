class SiteLiveData {
  final int ts;
  final int pvPower;
  final int gridPower;
  final int sitePower;

  SiteLiveData(this.ts, this.pvPower, this.gridPower, this.sitePower);

  static SiteLiveData? fromMap(Map<String, dynamic> json) {
    if (json.containsKey('timestamp') &&
        json.containsKey('pv_power') &&
        json.containsKey('grid_power')) {
      return SiteLiveData(json['timestamp'], json['pv_power'],
          json['grid_power'], -json['pv_power'] - json['grid_power']);
    }
    return null;
  }
}
