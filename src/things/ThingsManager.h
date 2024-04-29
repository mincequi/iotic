#pragma once

#include <cstdint>

#include <uvw_net/dns_sd//DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

namespace uvw_iot {
namespace common {
class ThingRepository;
} // namespace common
} // namespace uvw_iot

using uvw_iot::common::ThingRepository;
using uvw_net::dns_sd::DnsServiceDiscovery;
using uvw_net::modbus::ModbusDiscovery;
using uvw_net::sunspec::SunSpecDiscovery;

class Config;

// TODO: ThingsManager should probably not belong to things (due to cyclic dependencies).
class ThingsManager {
public:
    explicit ThingsManager(const ThingRepository& thingsRepository, const Config& cfg);

    void startDiscovery(int msec);

    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    //void endOfDayReached();

private:
    void onTimer() const;

    const ThingRepository& _thingRepository;
    const Config& _cfg;
    DnsServiceDiscovery _dnsDiscovery;
    SunSpecDiscovery _sunspecDiscovery;
    ModbusDiscovery _modbusDiscovery;

    uint64_t _currentTimestamp = 0;
};
