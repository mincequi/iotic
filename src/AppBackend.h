#pragma once

#include <QObject>

#include <influx/InfluxExporter.h>
#include <mqtt/MqttExporter.h>
#include <things/ThingManager.h>
#include <things/ThingsRepository.h>
#include <things/http/HttpDiscovery.h>
#include <things/sunspec/SunSpecManager.h>
#include <webserver/WebServer.h>

#include "Statistics.h"

using namespace std::placeholders;
using namespace sunspec;

class FeedManager;

class AppBackend : public QObject {
    Q_OBJECT
public:
    AppBackend();

private:
    Statistics _stats;
    ThingsRepository _thingsRepository;
    ThingManager _thingManager;
    sunspec::SunSpecManager _sunSpecManager;
    MqttExporter _mqttExporter;
    std::optional<InfluxExporter> _influxExporter;
    WebServer _webSocketExporter;
};
