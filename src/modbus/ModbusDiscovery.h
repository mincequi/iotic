#pragma once

#include <list>

#include <modbus/ModbusThing.h>
#include <things/ThingsDiscovery.h>

class ThingsRepository;

class ModbusDiscovery : public ThingsDiscovery {
public:
    ModbusDiscovery();

    void start(int msec) override;
    void stop() override;

private:
    void onCandidateStateChanged(const ModbusThingPtr& thing, Thing::State state);

    std::list<std::pair<ModbusThingPtr, rpp::composite_subscription>> _candidates;
};

using ModbusDiscoveryPtr = std::shared_ptr<ModbusDiscovery>;
