#pragma once

#include <QModbusDevice>

#include <things/Thing.h>

class QModbusDataUnit;
class QModbusReply;
class QModbusTcpClient;

class ModbusThing : public Thing {
public:
    ModbusThing(const ThingInfo& info);
    virtual ~ModbusThing();

    bool connect();
    void disconnect();

    QString host() const;

    QModbusReply* sendReadRequest(const QModbusDataUnit& read, int serverAddress);

private:
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);

    QModbusTcpClient* _modbusClient;
};

using ModbusThingPtr = std::shared_ptr<ModbusThing>;
