#pragma once

#include <cstdint>
#include <vector>

#include "ModbusRequest.h"

class ModbusResponse {
public:
    static ModbusResponse deserialize(const uint8_t buffer[], int length);

    uint16_t transactionId;
    uint16_t protocolId;
    uint8_t unitId;
    uint8_t functionCode;
    uint8_t exceptionCode = 0;

    uint16_t userData = 0;
    std::vector<std::uint16_t> values;

private:
    ModbusResponse();
};
