#include "AppBackend.h"

#include <config/Config.h>
#include <common/Util.h>
//#include "feed/FeedManager.h"
//#include "feed/LiveFeed.h"
#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModel.h>

#include <QCoreApplication>
#include <QThread>

using namespace sunspec;

AppBackend::AppBackend(/*FeedManager& feedManager*/)
    : //_feedManager(feedManager),
      _thingsManager(_thingsRepository),
      _sunSpecManager(_thingsRepository),
      _mqttExporter("broker.hivemq.com"),
      _rulesEngine(_thingsRepository),
      _webSocketExporter(_thingsRepository) {

    // Setup Statistics
    QObject::connect(&_stats, &Statistics::statsChanged, [&](const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model) {
        LOG_S(INFO) << thing.sunSpecId() << "> stats: " << model;
    });

    // Setup SunSpecManager
    /*
    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::modelRead, [&](const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
        LOG_S(1) << thing.sunSpecId() << "> " << model;
        _stats.feedModel(thing, model);
        if (model.modelId() == sunspec::Model::Id::MeterWyeConnectThreePhase &&
                model.values().count(sunspec::totalActiveAcPower)) {
            _feedManager.liveFeed().setGridMeterPower(thing.sunSpecId(), std::get<int32_t>(model.values().at(sunspec::totalActiveAcPower)));
        } else if (model.modelId() == sunspec::Model::Id::InverterThreePhase &&
                   model.values().count(sunspec::totalActiveAcPower)) {
            _feedManager.liveFeed().setInverterPower(thing.sunSpecId(), std::get<int32_t>(model.values().at(sunspec::totalActiveAcPower)));
        }
    });
    */

    /*
    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::thingDiscovered, [&](const sunspec::SunSpecThing& thing) {
        std::stringstream ss;
        for (const auto& kv : thing.models()) {
            ss << kv.first << "(" << kv.second.second << "), ";
        }
        LOG_S(INFO) << "thing discovered> id: " << thing.sunSpecId()
                    << ", host: " << thing.host()
                    << ", modbusUnitId: " << (uint32_t)thing.modbusUnitId()
                    << ", models: " << ss.str();

        if (thing.models().count(Model::Id::InverterSinglePhase)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), Model::Id::InverterSinglePhase, cfg->primaryInterval() });
        }
        if (thing.models().count(Model::Id::InverterThreePhase)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), Model::Id::InverterThreePhase, cfg->primaryInterval() });
        }
        if (thing.models().count(Model::Id::InverterMpptExtension)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), Model::Id::InverterMpptExtension, cfg->secondaryInterval() });
        }
        if (thing.models().count(Model::Id::MeterWyeConnectThreePhase)) {
            _sunSpecManager.addTask({ thing.sunSpecId(), Model::Id::MeterWyeConnectThreePhase, cfg->primaryInterval() });
        }
    });
    */

    QObject::connect(&_sunSpecManager, &sunspec::SunSpecManager::endOfDayReached, &_stats, &Statistics::reset);

    // Setup MqttExporter
    //QObject::connect(&_sunSpecManager, &SunSpecManager::modelRead, &_mqttExporter, &MqttExporter::exportLiveData);
    QObject::connect(&_stats, &Statistics::statsChanged, &_mqttExporter, &MqttExporter::exportStatsData);

#ifdef USE_INFLUXDB
    // Setup InfluxExporter
    const QString db = "elsewhere_" + util::getMacAddress().remove(':');
    _influxExporter = InfluxExporter::build(db.toStdString())
                      .host("localhost")
                      .port(8086);
    if (_influxExporter) {
        QObject::connect(&_sunSpecManager, &SunSpecManager::modelRead, std::bind(&InfluxExporter::exportLiveData, &_influxExporter.value(), _1, _2, _3));
    }
#endif

    // Start discovery
    _thingsManager.startDiscovery();
    _sunSpecManager.startDiscovery(60);
}

