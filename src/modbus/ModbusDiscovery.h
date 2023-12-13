#pragma once

#include <QTimer>

#include <uvw/tcp.h>

#include <things/ThingsDiscovery.h>
#include <things/sunspec/SunSpecThing.h>

class ThingsRepository;

namespace modbus {

class ModbusDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    ModbusDiscovery();

    void start(int msec) override;
    void stop() override;

private:
    void onStartDiscovering();
    void onCandidateStateChanged(const sunspec::SunSpecThing* thing, Thing::State state);

    QTimer _discoveryTimer;
    std::list<std::pair<std::shared_ptr<sunspec::SunSpecThing>, rpp::composite_subscription>> _candidates;
};

} // namespace modbus
