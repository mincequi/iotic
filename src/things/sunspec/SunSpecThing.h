#pragma once

#include <set>

#include <QModbusTcpClient>

#include <things/Thing.h>
#include <things/sunspec/models/SunSpecCommonModelFactory.h>
#include <things/sunspec/SunSpecLiveValue.h>

namespace sunspec {

class SunSpecThing : public Thing {
public:
    static ThingPtr from(const ThingInfo& info);
    SunSpecThing(const ThingInfo& info);
    ~SunSpecThing();

    QString host() const;
    uint8_t modbusUnitId() const;

    std::string manufacturer() const;
    std::string product() const;
    std::string serial() const;
    std::string sunSpecId() const;

    const std::map<uint16_t, std::pair<uint16_t, uint16_t>>& models() const;

    bool connect();
    void disconnect();

    bool isValid() const;
    bool hasCommonModel() const;

    bool readModel(uint16_t modelId, uint32_t timestamp);

    void reset();

private:
    void doRead() override;

    uint8_t nextUnitId();
    void pollNextUnitId();

    void readHeader(uint8_t id);
    void onReadHeader(QModbusReply* reply);

    void readModelTable(uint16_t address);
    void onReadModelTable(QModbusReply* reply);
    void addModelAddress(uint16_t modelId, uint16_t startAddress, uint16_t length);

    bool readBlock(uint16_t modelId, uint16_t address, uint16_t size, uint32_t timestamp);
    void onReadBlock(uint16_t modelId, uint32_t timestamp, QModbusReply* reply);
    void onReadBlockError(uint16_t modelId, QModbusReply* reply);

    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);

    void parseModel(uint16_t modelId, const std::vector<uint16_t>& buffer, uint32_t timestamp);

    static std::string toString(const LiveValue& v);

    QModbusTcpClient* _modbusClient;
    uint8_t _unitId = 0;
    uint8_t _unitIdx = 0;

    std::string _manufacturer;
    std::string _product;
    std::string _serial;
    std::string _sunSpecId;

    uint8_t _timeoutCount = 0;

    std::map<uint16_t, std::pair<uint16_t, uint16_t>> _modelAddresses;

    std::map<uint16_t, sunspec::Model> _models;
};

using SunSpecThingPtr = std::unique_ptr<SunSpecThing>;

} // namespace sunspec

