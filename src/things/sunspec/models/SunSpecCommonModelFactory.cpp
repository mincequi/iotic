#include "SunSpecCommonModelFactory.h"

#include <things/sunspec/SunSpecModel.h>

bool SunSpecCommonModelFactory::updateFromBuffer(sunspec::Model& model, const std::vector<uint16_t>& buffer) {
    if (buffer.size() != 66 && buffer.size() != 65) return false;

    model._values[sunspec::Manufacturer] = readString(buffer.data(), 16);
    model._values[sunspec::Product] = readString(buffer.data()+16, 16);
    model._values[sunspec::Options] = readString(buffer.data()+32, 8);
    model._values[sunspec::Version] = readString(buffer.data()+40, 8);
    model._values[sunspec::Serial] = readString(buffer.data()+48, 16);

    return true;
}

std::string SunSpecCommonModelFactory::readString(const uint16_t* begin, uint8_t length) {
    std::string out;
    out.reserve(length * 2);

    for (uint8_t i = 0; i < length; ++i) {
        if (*((uint8_t*)(begin+i)+1) != 0) {
            out.push_back(*((uint8_t*)(begin+i)+1));
        } else {
            break;
        }

        if (*(uint8_t*)(begin+i) != 0) {
            out.push_back(*(uint8_t*)(begin+i));
        } else {
            break;
        }
    }

    return out;
}
