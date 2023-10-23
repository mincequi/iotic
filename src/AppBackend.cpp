#include "AppBackend.h"

#include <config/Config.h>
#include <common/Util.h>
#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModel.h>

#include <QCoreApplication>
#include <QThread>

using namespace sunspec;

AppBackend::AppBackend()
    : _thingsRepository(ThingsRepository::instance()),
      _thingsManager(*_thingsRepository),
      _sunSpecManager(*_thingsRepository),
      _mqttExporter("broker.hivemq.com"),
      _rulesEngine(*_thingsRepository),
      _webSocketExporter(*_thingsRepository) {

    // Setup Statistics
    QObject::connect(&_stats, &Statistics::statsChanged, [&](const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model) {
        LOG_S(INFO) << thing.sunSpecId() << "> stats: " << model;
    });

    // Setup SunSpecManager
    /*
    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::modelRead, [&](const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
        LOG_S(1) << thing.sunSpecId() << "> " << model;
        _stats.feedModel(thing, model);
    });
    */

    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::endOfDayReached, &_stats, &Statistics::reset);

    // Setup MqttExporter
    //QObject::connect(&_sunSpecManager, &SunSpecManager::modelRead, &_mqttExporter, &MqttExporter::exportLiveData);
    QObject::connect(&_stats, &Statistics::statsChanged, &_mqttExporter, &MqttExporter::exportStatsData);

#ifdef USE_INFLUXDB
    // Setup InfluxExporter
    const QString db = "iotic_" + util::getMacAddress().remove(':');
    _influxExporter = InfluxExporter::build(db.toStdString())
                      .host("localhost")
                      .port(8086);
    if (_influxExporter) {
        QObject::connect(&_sunSpecManager, &SunSpecManager::modelRead, std::bind(&InfluxExporter::exportLiveData, &_influxExporter.value(), _1, _2, _3));
    }
#endif

    // Start discovery
    _thingsManager.startDiscovery(60 * 1000);
    _sunSpecManager.startDiscovery(60 * 1000);
}

