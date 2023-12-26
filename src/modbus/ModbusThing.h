#pragma once

#include <modbus/ModbusRequest.h>
#include <modbus/ModbusResponse.h>
#include <things/Thing.h>

namespace uvw {
class tcp_handle;
}

class ModbusThing : public Thing, public std::enable_shared_from_this<ModbusThing> {
public:
    ModbusThing(const ThingInfo& info);
    virtual ~ModbusThing();

    void connect();

    void readHoldingRegisters(uint8_t uintId, uint16_t address, uint16_t length, uint16_t userData = 0);

    dynamic_observable<ModbusResponse> holdingRegistersObservable() const;
    dynamic_observable<uint8_t> exceptionObservable() const;

private:
    publish_subject<ModbusResponse> _holdingRegistersSubject;
    publish_subject<uint8_t> _exceptionSubject;

    std::shared_ptr<uvw::tcp_handle> _tcpClient;

    ModbusRequest _request;

    std::map<uint16_t, uint16_t> _transactionsUserData;
};

using ModbusThingPtr_asio = std::shared_ptr<ModbusThing>;
