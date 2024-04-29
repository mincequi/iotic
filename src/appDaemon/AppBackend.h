#pragma once

#include <uvw_iot/common/ThingRepository.h>
//#include <influx/InfluxExporter.h>
//#include <mqtt/MqttExporter.h>
#include <config/Config.h>
#include <rules/RulesEngine.h>
#include <things/Site.h>
#include <things/ThingsManager.h>
#include <webserver/WebServer.h>

using namespace std::placeholders;
using uvw_iot::common::ThingRepository;

class AppBackend {
public:
    AppBackend();
    ~AppBackend();

private:
    ThingRepository _thingRepository;
    Config _cfg;
    ThingsManager _thingsManager;
    Site _site;
    RulesEngine _rulesEngine;
    //MqttExporter _mqttExporter;
    //std::optional<InfluxExporter> _influxExporter;
    WebServer _webServer;
};
