#include <QCoreApplication>
#include <QModbusTcpClient>
#include <QNetworkInterface>
#include <QTcpSocket>

#include "Logger.h"
#include "InfluxExporter.h"
#include "MqttExporter.h"
#include "Statistics.h"
#include "Util.h"
#include "sunspec/SunSpecManager.h"
#include "sunspec/SunSpecMeasuredValue.h"
#include "sunspec/SunSpecModel.h"
#include "sunspec/SunSpecThing.h"
#include "sunspec/SunSpecTypes.h"

using namespace std::placeholders;
using namespace sunspec;

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Setup Statistics
    Statistics stats;
    QObject::connect(&stats, &Statistics::statsChanged, [&](const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model) {
        LOG_S(INFO) << thing.sunSpecId() << "> stats: " << model;
    });

    // Setup SunSpecManager
    sunspec::SunSpecManager mgr;
    QObject::connect(&mgr, &sunspec::SunSpecManager::modelRead, [&](const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
        LOG_S(1) << thing.sunSpecId() << "> " << model;
        stats.feedModel(thing, model);
    });
    QObject::connect(&mgr, &sunspec::SunSpecManager::thingDiscovered, [&](const sunspec::SunSpecThing& thing) {
        std::stringstream ss;
        for (const auto& kv : thing.models()) {
            ss << kv.first << "(" << kv.second.second << "), ";
        }
        LOG_S(INFO) << "thing discovered: " << thing.sunSpecId() << "> "
                    << "host: " << thing.host()
                    << ", modbusUnitId: " << (uint32_t)thing.modbusUnitId()
                    << ", models: " << ss.str();

        if (thing.models().count(101)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 101, 3 });
        }
        if (thing.models().count(103)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 103, 3 });
        }
        if (thing.models().count(160)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 160, 3 });
        }
        if (thing.models().count(203)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 203, 3 });
        }
    });
    QObject::connect(&mgr, &sunspec::SunSpecManager::endOfDayReached, &stats, &Statistics::reset);

    // Setup MqttExporter
    MqttExporter mqttExporter("broker.hivemq.com");
    QObject::connect(&mgr, &SunSpecManager::modelRead, &mqttExporter, &MqttExporter::exportLiveData);
    QObject::connect(&stats, &Statistics::statsChanged, &mqttExporter, &MqttExporter::exportStatsData);

    // Setup InfluxExporter
    QString db = "elsewhere_" + util::getMacAddress().remove(':');
    std::optional<InfluxExporter> influxExporter = InfluxExporter::build(db.toStdString())
                                                   .host("localhost")
                                                   .port(8086);
    if (influxExporter) {
        QObject::connect(&mgr, &SunSpecManager::modelRead, std::bind(&InfluxExporter::exportLiveData, &influxExporter.value(), _1, _2, _3));
    }

    // Start discovery
    mgr.startDiscovering(60);

    return a.exec();
}
