#include <QCoreApplication>
#include <QModbusTcpClient>
#include <QNetworkInterface>
#include <QTcpSocket>

#include "Logger.h"
#include "MqttExporter.h"
#include "Statistics.h"
#include "sunspec/SunSpecManager.h"
#include "sunspec/SunSpecMeasuredValue.h"
#include "sunspec/SunSpecModel.h"
#include "sunspec/SunSpecThing.h"
#include "sunspec/SunSpecTypes.h"

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
        LOG_S(INFO) << "thing discovered: " << thing.sunSpecId() << "> "
                    << "host:" << thing.host()
                    << ", modbusUnitId: " << (uint32_t)thing.modbusUnitId();

        if (thing.models().contains(103)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 103, 3 });
        }
        if (thing.models().contains(160)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 160, 5 });
        }
        if (thing.models().contains(203)) {
            mgr.addTask({ thing.sunSpecId(), SunSpecManager::Task::Op::Read, 203, 3 });
        }
    });

    // Setup MqttExporter
    MqttExporter mqttExporter("broker.hivemq.com");
    QObject::connect(&mgr, &SunSpecManager::modelRead, &mqttExporter, &MqttExporter::exportLiveData);
    QObject::connect(&stats, &Statistics::statsChanged, &mqttExporter, &MqttExporter::exportStatsData);

    // Start discovery
    mgr.startDiscovering(60);

    return a.exec();
}
