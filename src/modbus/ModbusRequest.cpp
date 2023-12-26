#include "ModbusRequest.h"

#include <cstring>

ModbusRequest::ModbusRequest() {
}

const std::array<uint8_t, 12>& ModbusRequest::serialize() {
    static const uint16_t protocolId = 0x00;
    static const uint8_t length = 0x06;
    static const uint8_t functionCode = 0x03;

    ++transactionId;

    auto p = _buffer.data();
    std::memcpy(p, &transactionId, 2); p += 2;
    std::memcpy(p, &protocolId, 2); p += 2;
    *p = 0; p += 1;
    *p = length;; p += 1;
    *p = unitId; p += 1;
    *p = functionCode; p += 1;
    *p = (memoryAddress & 0xff00) >> 8; p += 1;
    *p = (memoryAddress & 0xff); p += 1;
    *p = (memoryLength & 0xff00) >> 8; p += 1;
    *p = (memoryLength & 0xff);

    return _buffer;
}
