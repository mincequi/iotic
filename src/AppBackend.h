#ifndef APPBACKEND_H
#define APPBACKEND_H

#include "InfluxExporter.h"
#include "Statistics.h"
#include "MqttExporter.h"
#include <iot/ThingManager.h>
#include <iot/http/HttpDiscovery.h>
#include <iot/sunspec/SunSpecManager.h>

using namespace std::placeholders;
using namespace sunspec;

class FeedManager;

class AppBackend {
public:
    AppBackend(/*FeedManager& liveFeed*/);

private:
    //FeedManager& _feedManager;
    Statistics _stats;
    ThingManager _thingManager;
    sunspec::SunSpecManager _sunSpecManager;
    MqttExporter _mqttExporter;
    std::optional<InfluxExporter> _influxExporter;
};


#endif // APPBACKEND_H
