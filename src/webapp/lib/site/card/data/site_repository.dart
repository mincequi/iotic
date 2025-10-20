import 'package:get/get.dart';
import 'package:iotic/logs/log_service.dart';
import 'package:iotic/site/card/data/site_data_historic.dart';
import 'package:iotic/site/card/data/site_data_live.dart';
import 'package:iotic/common/web_socket_data_source.dart';
import 'package:iotic/common/web_socket_handler.dart';

class SiteRepository implements WebSocketHandler {
  SiteRepository(this._wsDataSource) {
    _wsDataSource.registerHandler(this);
  }

  final WebSocketDataSource _wsDataSource;
  final LogService _logService = Get.find();

  final siteDataLive = SiteDataLive(0, 0, 0, 0).obs;
  final siteDataHistoric = SiteDataHistoric.empty().obs;

  @override
  bool onMessage(String id, Map<String, dynamic> props) {
    if (id != "site") return false;

    if (props.values.first is List) {
      siteDataHistoric.value = SiteDataHistoric.fromMap(props);
      siteDataLive.value = SiteDataLive(
          siteDataHistoric.value.ts.last,
          siteDataHistoric.value.pvPower.last,
          siteDataHistoric.value.gridPower.last,
          -siteDataHistoric.value.pvPower.last -
              siteDataHistoric.value.gridPower.last);
      _logService.info(
          "SiteRepository.onMessage: history from ${siteDataHistoric.value.ts.first} to ${siteDataHistoric.value.ts.last}");
      // TODO: return false for now, because it should be further handled
      return false;
    }

    _logService.setSiteProperties(props);
    //_logService.info("SiteRepository.onMessage: $props");
    // TODO: return false for now, because it should be further handled
    return false;
  }
}
