#include <QCoreApplication>
#include <QModbusTcpClient>
#include <QNetworkInterface>
#include <QTcpSocket>

#include <magic_enum.hpp>

#include "Logger.h"
#include "MqttExporter.h"
#include "sunspec/SunSpecManager.h"
#include "sunspec/SunSpecMeasuredValue.h"
#include "sunspec/SunSpecThing.h"

template <class T>
QDebug operator<<(QDebug debug, const SunSpecMeasuredValue<T>& value) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "{ \"curr\": " << value.curr
                    << ", \"min\": " << value.min
                    << ", \"max\": " << value.max << " }";

    return debug;
}

int main(int argc, char *argv[]) {
    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Setup SunSpecManager
    SunSpecManager mgr;
    QObject::connect(&mgr, &SunSpecManager::modelRead, [&](SunSpecThing* thing, const SunSpecModel& model) {
        if (std::holds_alternative<SunSpecWyeConnectMeterModel>(model)) {
            auto meter = std::get<SunSpecWyeConnectMeterModel>(model);
            //LOG_S(INFO) << thing->sunSpecId() << "> " << meter.values();
        } else if (std::holds_alternative<SunSpecMpptInverterExtensionModel>(model)) {
            auto inv = std::get<SunSpecMpptInverterExtensionModel>(model);
            //LOG_S(INFO) << thing->sunSpecId() << "> dcPower: " << inv.dcPowers;
        } else if (std::holds_alternative<SunSpecInverterModel>(model)) {
            auto inv = std::get<SunSpecInverterModel>(model);
            LOG_S(INFO) << thing->sunSpecId() << "> " << inv.values();
        }
    });
    QObject::connect(&mgr, &SunSpecManager::thingDiscovered, [&](SunSpecThing* thing) {
        LOG_S(INFO) << "thing discovered> host:" << thing->host()
                    << ", modbusUnitId: " << (uint32_t)thing->modbusUnitId()
                    << ", sunSpecId: " << thing->sunSpecId();
    });

    // Setup MqttExporter
    MqttExporter mqttExporter("minskomat");
    QObject::connect(&mgr, &SunSpecManager::modelRead, &mqttExporter, &MqttExporter::exportSunSpecModel);

    // Some example tasks
    mgr.addTask({ "elgris_smart_meter_1900042748", SunSpecManager::Task::Op::Read, 203, 1 });
    mgr.addTask({ "sma_solar_inverter_1980417100", SunSpecManager::Task::Op::Read, 103, 3 });
    mgr.addTask({ "sma_solar_inverter_3006932172", SunSpecManager::Task::Op::Read, 103, 3 });
    mgr.addTask({ "sma_solar_inverter_1980417100", SunSpecManager::Task::Op::Read, 160, 5 });
    mgr.addTask({ "sma_solar_inverter_3006932172", SunSpecManager::Task::Op::Read, 160, 5 });

    // Start discovery
    mgr.startDiscovering(60);

    return a.exec();
}
