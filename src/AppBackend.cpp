#include "AppBackend.h"

#include "Logger.h"
#include "Util.h"

#include <QThread>

using namespace std::placeholders;
using namespace sunspec;

void AppBackend::run(std::atomic_bool& doRun) {
}

AppBackend::AppBackend()
    : _mqttExporter("broker.hivemq.com") {

    // Setup Statistics
    QObject::connect(&_stats, &Statistics::statsChanged, [&](const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model) {
        LOG_S(INFO) << thing.sunSpecId() << "> stats: " << model;
    });

    // Setup SunSpecManager
    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::modelRead, [&](const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
        LOG_S(1) << thing.sunSpecId() << "> " << model;
        _stats.feedModel(thing, model);
    });
    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::thingDiscovered, [&](const sunspec::SunSpecThing& thing) {
        std::stringstream ss;
        for (const auto& kv : thing.models()) {
            ss << kv.first << "(" << kv.second.second << "), ";
        }
        LOG_S(INFO) << "thing discovered: " << thing.sunSpecId() << "> "
                    << "host: " << thing.host()
                    << ", modbusUnitId: " << (uint32_t)thing.modbusUnitId()
                    << ", models: " << ss.str();

        if (thing.models().count(101)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 101, 2500 });
        }
        if (thing.models().count(103)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 103, 2500 });
        }
        if (thing.models().count(160)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 160, 2500 });
        }
        if (thing.models().count(203)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 203, 2500 });
        }
    });
    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::endOfDayReached, &_stats, &Statistics::reset);

    // Setup MqttExporter
    QObject::connect(&_sunSpecManager, &SunSpecManager::modelRead, &_mqttExporter, &MqttExporter::exportLiveData);
    QObject::connect(&_stats, &Statistics::statsChanged, &_mqttExporter, &MqttExporter::exportStatsData);

#ifdef USE_INFLUXDB
    // Setup InfluxExporter
    const QString db = "elsewhere_" + util::getMacAddress().remove(':');
    std::optional<InfluxExporter> influxExporter = InfluxExporter::build(db.toStdString())
                                                   .host("localhost")
                                                   .port(8086);
    if (influxExporter) {
        QObject::connect(&_sunSpecManager, &SunSpecManager::modelRead, std::bind(&InfluxExporter::exportLiveData, &influxExporter.value(), _1, _2, _3));
    }
#endif

    // Start discovery
    _sunSpecManager.startDiscovering(60);
}

