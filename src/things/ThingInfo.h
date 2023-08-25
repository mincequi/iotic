#pragma once

#include <string>

namespace modbus {
class ModbusDiscovery;
}

class ThingInfo {
public:
    enum DiscoveryType {
        Invalid,
        SunSpec,
        Http
    };

    ThingInfo(DiscoveryType type,
              const std::string& id,
              const std::string& host = "");

    bool isValid() const;
    DiscoveryType discoveryType() const;
    std::string id() const;
    std::string host() const;

protected:
    DiscoveryType _discoveryType = DiscoveryType::Invalid;

private:
    std::string _id;
    std::string _host;

    // Custom fields
    std::string _name;

    friend class modbus::ModbusDiscovery;
};
