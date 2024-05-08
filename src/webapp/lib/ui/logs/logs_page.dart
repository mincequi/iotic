import 'package:flutter/material.dart';

import 'package:get/get.dart';
import 'package:iotic/data/logs/log_level.dart';
import 'package:iotic/data/logs/logging_service.dart';
import 'package:iotic/ui/iotic_theme.dart';

class LogsPage extends StatelessWidget {
  final LoggingService logger = Get.find();

  LogsPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Obx(
      () {
        final filteredLogs = logger.logs
            .where((logEntry) => logger.selectedLevels.contains(logEntry.level))
            .toList();
        return ListView.builder(
          padding: EdgeInsets.zero, // Remove padding
          physics: const RangeMaintainingScrollPhysics(),
          //reverse: true,
          itemCount: filteredLogs.length,
          itemBuilder: (context, index) {
            var log = filteredLogs[index];
            return Padding(
              padding: const EdgeInsets.symmetric(vertical: 0),
              child: Text(log.message,
                  style: TextStyle(
                      fontSize: 8,
                      color: (log.level == LogLevel.error)
                          ? IoticTheme.orange
                          : (log.level == LogLevel.warning)
                              ? IoticTheme.yellow
                              : Colors.white)),
            );
          },
        );
      },
    );
  }
}
