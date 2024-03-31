#pragma once

//#include <influx/InfluxExporter.h>
//#include <mqtt/MqttExporter.h>
#include <things/CandidatesRepository.h>
#include <things/ThingsManager.h>
#include <things/ThingsRepository.h>
#include <webserver/WebServer.h>

#include "Statistics.h"

using namespace std::placeholders;
using namespace sunspec;

class AppBackend {
public:
    AppBackend();

private:
    CandidatesRepository _candidatesRepository;
    ThingsRepository* _thingsRepository;
    ThingsManager _thingsManager;
    //MqttExporter _mqttExporter;
    //std::optional<InfluxExporter> _influxExporter;
    WebServer _webServer;
};
