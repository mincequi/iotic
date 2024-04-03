#pragma once

#include <modbus/ModbusDiscovery.h>
#include <things/Thing.h>
#include <things/ThingsDiscovery.h>

class CandidatesRepository;
class ThingsRepository;

// TODO: ThingsManager should probably not belong to things (due to cyclic dependencies).
class ThingsManager {
public:
    explicit ThingsManager(CandidatesRepository& candidatesRepository,
                           ThingsRepository& thingsRepository);

    void startDiscovery(int msec);
    void stopDiscovery();

    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    //void endOfDayReached();

private:
    void onTimer();
    void onDiscoveryTimer();

    CandidatesRepository& _candidatesRepository;
    ThingsRepository& _thingsRepository;

    uint64_t _currentTimestamp = 0;

    std::list<ThingsDiscoveryPtr> _discoveries;
    ModbusDiscoveryPtr _modbusDiscovery;
};
