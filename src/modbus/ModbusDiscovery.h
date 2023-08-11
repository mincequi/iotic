#pragma once

#include <QList>
#include <QTimer>
#include <things/ThingsDiscovery.h>
#include <things/sunspec/SunSpecThing.h>

namespace sunspec {
class SunSpecThing;
class SunSpecManager;
}

namespace modbus {

class ModbusDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    ModbusDiscovery(sunspec::SunSpecManager& manager);

    void start(uint16_t seconds = 60) override;
    void stop() override;

private:
    void onStartDiscovering();
    void onCandidateStateChanged(sunspec::SunSpecThing::State state);

    sunspec::SunSpecManager& _manager;
    QTimer _discoveryTimer;
    QList<sunspec::SunSpecThing*> _candidates;
};

} // namespace modbus
