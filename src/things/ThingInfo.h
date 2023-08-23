#pragma once

#include <string>

namespace modbus {
class ModbusDiscovery;
}

class ThingInfo {
public:
    enum Type {
        Invalid,
        SunSpec,
        Http
    };

    ThingInfo(Type type,
              const std::string& id,
              const std::string& host = "");

    bool isValid() const;
    Type type() const;
    std::string id() const;
    std::string host() const;

    // Custom properties which are loaded from config
    std::string name() const;

protected:
    Type _type = Type::Invalid;

private:
    std::string _id;
    std::string _host;

    // Custom fields
    std::string _name;

    friend class modbus::ModbusDiscovery;
};
