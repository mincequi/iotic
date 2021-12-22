#include "SunSpecCommonModelFactory.h"

#include <sunspec/SunSpecModel.h>

namespace sunspec {

bool SunSpecCommonModelFactory::updateFromBuffer(Model& model,
                                                 const std::vector<uint16_t>& buffer,
                                                 uint32_t /*timestamp*/) {
    if (buffer.size() != 66 && buffer.size() != 65) return false;

    model.m_values[sunspec::manufacturer] = readString(buffer.data(), 16);
    model.m_values[sunspec::product] = readString(buffer.data()+16, 16);
    model.m_values[sunspec::options] = readString(buffer.data()+32, 8);
    model.m_values[sunspec::version] = readString(buffer.data()+40, 8);
    model.m_values[sunspec::serial] = readString(buffer.data()+48, 16);

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

} // namespace sunspec
