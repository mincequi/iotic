class SiteLiveData {
  final int pvPower;
  final int gridPower;
  final int sitePower;

  SiteLiveData(this.pvPower, this.gridPower, this.sitePower);

  SiteLiveData.fromMap(Map<String, dynamic> json)
      : pvPower = json['pvPower'],
        gridPower = json['gridPower'],
        sitePower = json['sitePower'];
}
