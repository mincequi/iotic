#include "AppBackend.h"

#include <uvw_iot/util/Filter.h>

#include <HttpResponse.h>
#include <common/Logger.h>
#include <config/ConfigRepository.h>
#include <rules/RulesEngine.h>

using namespace uvw_iot::util;

AppBackend::AppBackend() :
    _configRepository(_thingRepository),
    _thingsManager(_thingRepository, _configRepository),
    _strategyRepository(_thingRepository),
    _site(_thingRepository, {.shortTermTau = 15000ms, .longTermTau = 180000ms}),
    //_mqttExporter("broker.hivemq.com"),
    _webServer(_thingRepository, _site, _configRepository, _strategyRepository, _symbolRepository),
    _rulesEngine(_thingRepository, _strategyRepository, _symbolRepository, _configRepository),
    _powerManager(_strategyRepository, _symbolRepository, _site, _configRepository) {

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

    _webServer.registerGetRoute("/discover", std::move([this](uWS::HttpResponse<false>* res, uWS::HttpRequest*) {
        _thingsManager.discover();
        res->writeHeader("Access-Control-Allow-Origin", "*");
        res->end("discovery started");
    }));
}

AppBackend::~AppBackend() {
}

