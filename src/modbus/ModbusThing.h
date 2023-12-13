#pragma once

#include <QModbusDevice>

#include <things/Thing.h>

class QModbusTcpClient;

class ModbusThing : public Thing {
public:
    ModbusThing(const ThingInfo& info);
    virtual ~ModbusThing();

    bool connect();
    void disconnect();

private:
    virtual void doRead() override;

    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);

    QModbusTcpClient* _modbusClient;
};
