#pragma once

#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/util/Site.h>
//#include <influx/InfluxExporter.h>
//#include <mqtt/MqttExporter.h>
#include <config/Config.h>
#include <rules/RulesEngine.h>
#include <strategies/StrategyRepository.h>
#include <things/ThingsManager.h>
#include <webserver/WebServer.h>

using namespace std::placeholders;
using uvw_iot::ThingRepository;

class AppBackend {
public:
    AppBackend();
    ~AppBackend();

private:
    ThingRepository _thingRepository;
    StrategyRepository _strategyRepository;
    Config _cfg;
    ThingsManager _thingsManager;
    Site _site;
    RulesEngine _rulesEngine;
    //MqttExporter _mqttExporter;
    //std::optional<InfluxExporter> _influxExporter;
    WebServer _webServer;
};
