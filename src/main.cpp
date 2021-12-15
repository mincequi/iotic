#include <QCoreApplication>
#include <QModbusTcpClient>
#include <QNetworkInterface>
#include <QTcpSocket>

#include <magic_enum.hpp>

#include "Logger.h"
#include "MqttExporter.h"
#include "Statistics.h"
#include "sunspec/SunSpecManager.h"
#include "sunspec/SunSpecMeasuredValue.h"
#include "sunspec/SunSpecModel.h"
#include "sunspec/SunSpecThing.h"

template <class T>
QDebug operator<<(QDebug debug, const sunspec::SunSpecMeasuredValue<T>& value) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "{ \"curr\": " << value.curr
                    << ", \"min\": " << value.min
                    << ", \"max\": " << value.max << " }";

    return debug;
}

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
        LOG_S(INFO) << thing.sunSpecId() << "> " << model;
        stats.feedModel(thing, model);
    });
    QObject::connect(&mgr, &sunspec::SunSpecManager::thingDiscovered, [&](const sunspec::SunSpecThing& thing) {
        LOG_S(INFO) << "thing discovered> host:" << thing.host()
                    << ", modbusUnitId: " << (uint32_t)thing.modbusUnitId()
                    << ", sunSpecId: " << thing.sunSpecId();
    });

    // Setup MqttExporter
    MqttExporter mqttExporter("minskomat");
    QObject::connect(&mgr, &SunSpecManager::modelRead, &mqttExporter, &MqttExporter::exportLiveData);
    QObject::connect(&stats, &Statistics::statsChanged, &mqttExporter, &MqttExporter::exportStatsData);

    // Some example tasks
    mgr.addTask({ "elgris_smart_meter_1900042748", SunSpecManager::Task::Op::Read, 203, 3 });
    mgr.addTask({ "sma_solar_inverter_1980417100", SunSpecManager::Task::Op::Read, 103, 3 });
    mgr.addTask({ "sma_solar_inverter_3006932172", SunSpecManager::Task::Op::Read, 103, 3 });
    mgr.addTask({ "sma_solar_inverter_1980417100", SunSpecManager::Task::Op::Read, 160, 5 });
    mgr.addTask({ "sma_solar_inverter_3006932172", SunSpecManager::Task::Op::Read, 160, 5 });

    // Start discovery
    mgr.startDiscovering(60);

    return a.exec();
}
