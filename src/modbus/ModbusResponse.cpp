#include "ModbusResponse.h"

#include <cstring>

ModbusResponse ModbusResponse::deserialize(const uint8_t buffer[], int length) {
    //std::vector<uint1

    auto p = buffer;
    ModbusResponse response;

    std::memcpy(&response.transactionId, p, 2); p += 2;
    std::memcpy(&response.protocolId, p, 2); p += 2;
    p += 2;
    response.unitId = *p; p += 1;
    response.functionCode = *p; p += 1;
    if (response.functionCode & 0x80) {
        response.exceptionCode = *p;
        return response;
    }
    uint8_t size = *p/2; p += 1;
    response.values.resize(size);
    for (uint8_t i = 0; i < size; ++i) {
        uint16_t v;
        std::memcpy(&v, p, 2);
        response.values[i] = ((v & 0xff) << 8) | ((v & 0xff00) >> 8);
        p += 2;
    }

    return response;
}

ModbusResponse::ModbusResponse() {
}
