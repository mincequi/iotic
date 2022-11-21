#ifndef APPBACKEND_H
#define APPBACKEND_H

#include "InfluxExporter.h"
#include "Statistics.h"
#include "MqttExporter.h"
#include "sunspec/SunSpecManager.h"

using namespace std::placeholders;
using namespace sunspec;

class AppBackend {
public:
    static void run(std::atomic_bool& doRun);

    AppBackend();

private:
    Statistics _stats;
    sunspec::SunSpecManager _sunSpecManager;
    MqttExporter _mqttExporter;
    std::optional<InfluxExporter> _influxExporter;
};


#endif // APPBACKEND_H
