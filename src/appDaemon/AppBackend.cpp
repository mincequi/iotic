#include "AppBackend.h"

#include <uvw_iot/util/Filter.h>

#include <common/Logger.h>
#include <config/Config.h>
#include <rules/RulesEngine.h>

using namespace uvw_iot::util;

static SiteConfig siteConfig = {
    .shortTermTau = 15000ms,
    .longTermTau = 40000ms,
};

AppBackend::AppBackend() :
    _cfg(_thingRepository),
    _thingsManager(_thingRepository, _cfg),
    _site(_thingRepository, {.shortTermTau = 15000ms, .longTermTau = 40000ms}),
    //_mqttExporter("broker.hivemq.com"),
    _webServer(_thingRepository, _site, _cfg),
    _rulesEngine(_thingRepository, _site, _cfg) {

    _thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
        LOG_S(INFO) << "thing added> " << thing->id() << ", type: " << thing->type();
    });
    _thingRepository.thingRemoved().subscribe([this](const auto& id) {
        LOG_S(INFO) << "thing removed> " << id;
    });

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
}

AppBackend::~AppBackend() {
}

