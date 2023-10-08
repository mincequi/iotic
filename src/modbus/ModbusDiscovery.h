#pragma once

#include <QList>
#include <QTimer>
#include <things/ThingsDiscovery.h>
#include <things/sunspec/SunSpecThing.h>

class ThingsRepository;

namespace sunspec {
class SunSpecThing;
class SunSpecManager;
}

namespace modbus {

class ModbusDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    ModbusDiscovery(//sunspec::SunSpecManager& manager,
                    ThingsRepository& repository);

    void start(int msec) override;
    void stop() override;

private:
    void onStartDiscovering();
    //void onCandidateStateChanged(sunspec::SunSpecThing::State state);
    void onCandidateStateChangedRpp(sunspec::SunSpecThing* thing, sunspec::SunSpecThing::State state);

    //sunspec::SunSpecManager& _manager;
    ThingsRepository& _thingsRepository;
    QTimer _discoveryTimer;
    QList<std::pair<sunspec::SunSpecThing*, rpp::composite_subscription>> _candidates;
};

} // namespace modbus
