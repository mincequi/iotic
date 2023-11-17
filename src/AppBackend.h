#pragma once

//#include <influx/InfluxExporter.h>
#include <mqtt/MqttExporter.h>
#include <things/ThingsManager.h>
#include <things/ThingsRepository.h>
#include <things/sunspec/SunSpecManager.h>
#include <webserver/WebServer.h>

#include "Statistics.h"

using namespace std::placeholders;
using namespace sunspec;

class AppBackend {
public:
    AppBackend();

private:
    ThingsRepository* _thingsRepository;
    ThingsManager _thingsManager;
    sunspec::SunSpecManager _sunSpecManager;
    MqttExporter _mqttExporter;
    //std::optional<InfluxExporter> _influxExporter;
    WebServer _webServer;
};
