import 'package:mqtt_client/mqtt_browser_client.dart';

enum MqttStatus { connecting, disconnected, connected, error }

class MqttService {
  MqttService._privateConstructor();

  static MqttService? _instance;

  factory MqttService() {
    if (_instance == null) {
      _instance = MqttService._privateConstructor();
      _instance?._client.logging(on: false);
      _instance?._client.setProtocolV311();
      _instance?._client.keepAlivePeriod = 20;
      //_instance?._client.updates
    }
    return _instance!;
  }

  MqttStatus _status = MqttStatus.disconnected;
  MqttStatus get status {
    return _status;
  }

  set status(MqttStatus status) {
    print('status changed: $status');
    _status = status;
    onStatus?.call();
  }

  void Function()? onStatus;

  void onConnected() {
    status = MqttStatus.connected;
  }

  void onDisconnected() {
    status = MqttStatus.disconnected;
  }

  String host() => _client.server;
  int port() => _client.port!;

  void connect(String host, int port) async {
    //if (_client.connectionStatus?.state == MqttConnectionState.connected &&
    //    _client.server == host &&
    //    _client.port == port) {
    //  return true;
    //}

    //ClientInformation info = await ClientInformation.fetch();
    //_client.clientIdentifier = info.deviceId;
    //_client.server = 'ws://' + host;
    //_client.port = port;
    _client.onConnected = onConnected;
    _client.onDisconnected = onDisconnected;

    try {
      status = MqttStatus.connecting;
      await _client.connect();
    } on Exception catch (e) {
      print('EXAMPLE::client exception - $e');
      _client.disconnect();
      status = MqttStatus.error;
    }
  }

  final MqttBrowserClient _client =
      MqttBrowserClient.withPort('ws://broker.hivemq.com', 'dsfsdf', 8000);
}
