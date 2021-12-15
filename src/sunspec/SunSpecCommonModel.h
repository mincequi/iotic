#ifndef SUNSPECCOMMONMODEL_H
#define SUNSPECCOMMONMODEL_H

#include <optional>
#include <string>
#include <vector>

#include "SunSpecModel.h"

class SunSpecCommonModel : public sunspec::Model {
public:
    static void updateFromBuffer(std::optional<SunSpecCommonModel>& model, const std::vector<uint16_t>& buffer);

    std::string manufacturer() const;
    std::string product() const;
    std::string options() const;
    std::string version() const;
    std::string serial() const;

private:
    SunSpecCommonModel();

    static std::string readString(const uint16_t* begin, uint8_t length);

    std::string m_manufacturer;
    std::string m_product;
    std::string m_options;
    std::string m_version;
    std::string m_serial;
};

#endif // SUNSPECCOMMONMODEL_H
