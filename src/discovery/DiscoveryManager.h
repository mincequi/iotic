#pragma once

#include <uvw_net/dns_sd//DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

#include <coap/CoapDiscovery.h>

namespace uvw_iot {
class ThingRepository;
} // namespace uvw_iot

using uvw_iot::ThingRepository;
using uvw_net::dns_sd::DnsServiceDiscovery;
using uvw_net::modbus::ModbusDiscovery;
using uvw_net::sunspec::SunSpecDiscovery;

class DiscoveryManager {
public:
    DiscoveryManager(const ThingRepository& thingRepository);

    void startDiscovery(int msec);

private:
    const ThingRepository& _thingRepository;

    CoapDiscovery _coapDiscovery;
    DnsServiceDiscovery _dnsDiscovery;
    SunSpecDiscovery _sunspecDiscovery;
    ModbusDiscovery _modbusDiscovery;
};
