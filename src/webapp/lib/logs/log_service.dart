import 'package:get/get.dart';
import 'package:iotic/logs/log.dart';
import 'package:iotic/logs/log_level.dart';

class LogService extends GetxService {
  final siteProperties = <String, dynamic>{}.obs;
  final logs = <Log>[].obs; // Observable list of logs

  final logLevel = LogLevel.warning.obs;

  final selectedLevels = [LogLevel.info, LogLevel.warning, LogLevel.error];

  void setLogLevel(LogLevel level) {
    logLevel.value = level;
  }

  void info(String message) {
    logs.insert(0, Log(LogLevel.info, message));
  }

  void warn(String message) {
    logs.insert(0, Log(LogLevel.warning, message));
  }

  void err(String message) {
    logs.insert(0, Log(LogLevel.error, message));
  }

  void setSiteProperties(Map<String, dynamic> props) {
    siteProperties.value = props;
  }
}
