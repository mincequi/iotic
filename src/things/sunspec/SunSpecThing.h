#pragma once

#include <modbus/ModbusThing.h>
#include <things/sunspec/SunSpecLiveValue.h>
#include <things/sunspec/SunSpecModel.h>

namespace sunspec {

/**
 * Implementation of a SunSpec thing via modbus
 *
 * At construction time this thing will poll any modbus unitId (in a strategic
 * order) for a valid SunSpec device. If it finds something it will check for
 * supported models.
 * A modbus host can actually house multiple modbus things (1 to 247). However,
 * we stop polling after finding the first valid unitId.
 * If we want to check for further unitIds, this has to be part of higher level
 * classes/implementations (e.g. FroniusDiscovery).
 */
class SunSpecThing : public Thing {
public:
    enum OperationId : uint16_t {
        ValidStart = 40000,
        ValidEnd = 49999,
        ReadHeader = 63000,
        ReadModelEntry = 63001
    };

    SunSpecThing(ModbusThingPtr modbusThing);
    ~SunSpecThing();

    std::string host() const;
    uint8_t unitId() const;

    std::string manufacturer() const;
    std::string product() const;
    std::string serial() const;
    std::string sunSpecId() const;

    const std::map<uint16_t, std::pair<uint16_t, uint16_t>>& models() const;

    bool readModel(uint16_t modelId);

private:
    void doRead() override;

    uint8_t nextUnitId();
    void pollNextUnitId();

    void readHeader(uint8_t id);
    std::optional<State> onReadHeader(const ModbusResponse& response);

    void readModelEntry(uint16_t address);
    std::optional<State> onReadModelEntry(const ModbusResponse& response);
    void addModelAddress(uint16_t modelId, uint16_t startAddress, uint16_t length);

    void readBlock(uint16_t modelId, uint16_t address, uint16_t size);
    void onReadBlock(const ModbusResponse& response);
    void parseBlock(uint16_t modelId, const std::vector<uint16_t>& buffer);

    static std::string toString(const sunspec::LiveValue& v);

    ModbusThingPtr _modbusThing;
    uint8_t _unitId = 0;
    uint8_t _unitIdx = 0;

    uint16_t _headerLength = 0;
    std::string _manufacturer;
    std::string _product;
    std::string _serial;
    std::string _sunSpecId;

    uint8_t _timeoutCount = 0;

    std::map<uint16_t, std::pair<uint16_t, uint16_t>> _blockAddresses;

    std::map<uint16_t, sunspec::Model> _models;

    friend class FroniusDiscovery;
};

} // namespace sunspec

using SunSpecThingPtr = std::shared_ptr<sunspec::SunSpecThing>;
