#pragma once

#include <array>
#include <stdint.h>

class ModbusRequest {
public:
    ModbusRequest();

    uint16_t transactionId = 0x00;
    uint8_t unitId;
    uint16_t memoryAddress;
    uint16_t memoryLength;

    const std::array<uint8_t, 12>& serialize();

private:
    std::array<uint8_t, 12> _buffer;
};
