import 'package:flutter/material.dart';

import 'package:get/get.dart';
import 'package:iotic/logs/log_level.dart';
import 'package:iotic/logs/log_service.dart';
import 'package:iotic/common/iotic_theme.dart';

class LogsPage extends StatelessWidget {
  final LogService _logService = Get.find();

  LogsPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Padding(
          padding: EdgeInsets.symmetric(vertical: 0),
          child:
              Text("Site", style: TextStyle(fontSize: 8, color: Colors.white)),
        ),
        Obx(
          () {
            return ListView.builder(
              padding: const EdgeInsets.only(left: 8), // Remove padding
              shrinkWrap: true,
              physics: const NeverScrollableScrollPhysics(),
              //reverse: true,
              itemCount: _logService.siteProperties.length,
              itemBuilder: (context, index) {
                var log = _logService.siteProperties.keys.elementAt(index) +
                    ": " +
                    _logService.siteProperties.values
                        .elementAt(index)
                        .toString();
                return Padding(
                  padding: const EdgeInsets.symmetric(vertical: 0),
                  child: Text(log,
                      style: const TextStyle(fontSize: 8, color: Colors.white)),
                );
              },
            );
          },
        ),
        Expanded(child: Obx(
          () {
            final filteredLogs = _logService.logs
                .where((logEntry) =>
                    _logService.selectedLevels.contains(logEntry.level))
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
        ))
      ],
    );
  }
}
