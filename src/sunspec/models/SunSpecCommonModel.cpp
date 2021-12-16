#include "SunSpecCommonModel.h"

void SunSpecCommonModel::updateFromBuffer(std::optional<SunSpecCommonModel>& model, const std::vector<uint16_t>& buffer) {
    if (buffer.size() != 66 && buffer.size() != 65) return;

    if (!model) {
        model.emplace(SunSpecCommonModel());
        model->m_modelId = 1;
    }

    model.value().m_manufacturer = readString(buffer.data(), 16);
    model.value().m_product = readString(buffer.data()+16, 16);
    model.value().m_options = readString(buffer.data()+32, 8);
    model.value().m_version = readString(buffer.data()+40, 8);
    model.value().m_serial = readString(buffer.data()+48, 16);
}

SunSpecCommonModel::SunSpecCommonModel() {
}

std::string SunSpecCommonModel::manufacturer() const {
    return m_manufacturer;
}

std::string SunSpecCommonModel::product() const {
    return m_product;
}

std::string SunSpecCommonModel::options() const {
    return m_options;
}

std::string SunSpecCommonModel::version() const {
    return m_version;
}

std::string SunSpecCommonModel::serial() const {
    return m_serial;
}

std::string SunSpecCommonModel::readString(const uint16_t* begin, uint8_t length) {
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
