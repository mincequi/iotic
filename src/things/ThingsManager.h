#pragma once

#include <QObject>
#include <QTimer>
#include <modbus/ModbusDiscovery.h>
#include <things/Thing.h>
#include <things/ThingsDiscovery.h>

class CandidatesRepository;
class ThingsRepository;

class ThingsManager : public QObject {
    Q_OBJECT
public:
    explicit ThingsManager(CandidatesRepository& candidatesRepository,
                           ThingsRepository& thingsRepository,
                           QObject *parent = nullptr);

    void startDiscovery(int msec);
    void stopDiscovery();

signals:
    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    void endOfDayReached();

private:
    void onTimer();
    void onDiscoveryTimer();

    CandidatesRepository& _candidatesRepository;
    ThingsRepository& _thingsRepository;

    QTimer _timer;
    QTimer _discoveryTimer;
    uint64_t _currentTimestamp = 0;

    std::list<ThingsDiscoveryPtr> _discoveries;
    ModbusDiscoveryPtr _modbusDiscovery;
};
