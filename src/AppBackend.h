#pragma once

#include <QObject>

#include <influx/InfluxExporter.h>
#include <iot/ThingManager.h>
#include <iot/http/HttpDiscovery.h>
#include <iot/sunspec/SunSpecManager.h>
#include <mqtt/MqttExporter.h>

#include "Statistics.h"

using namespace std::placeholders;
using namespace sunspec;

class FeedManager;

class AppBackend : public QObject {
    Q_OBJECT
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
