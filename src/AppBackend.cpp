#include "AppBackend.h"

#include <config/Config.h>
#include <common/Util.h>
#include <rules/RulesEngine.h>
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
      _webServer(*_thingsRepository) {
    // We define order of singleton instantiations here.
    rules;

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

