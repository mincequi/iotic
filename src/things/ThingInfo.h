#pragma once

#include <string>

class ThingInfo {
public:
    enum DiscoveryType {
        Invalid,
        SunSpec,
        Http
    };

    ThingInfo(DiscoveryType type,
              const std::string& id,
              const std::string& host);
    virtual ~ThingInfo();

    bool isValid() const;
    DiscoveryType discoveryType() const;
    std::string id() const;
    std::string host() const;

protected:
    DiscoveryType _discoveryType = DiscoveryType::Invalid;

private:
    std::string _id;
    std::string _host;

    friend class ModbusDiscovery;
    friend class SunSpecDiscovery;
};
