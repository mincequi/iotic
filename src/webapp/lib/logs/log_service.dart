import 'package:get/get.dart';
import 'package:iotic/logs/log.dart';
import 'package:iotic/logs/log_level.dart';

class LogService extends GetxService {
  final RxList<Log> logs = <Log>[].obs; // Observable list of logs

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
}
