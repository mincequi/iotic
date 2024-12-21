#pragma once

#include <list>
#include <uvw_net/dns_sd//DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

using uvw_net::dns_sd::DnsServiceDiscovery;
using uvw_net::modbus::ModbusDiscovery;
using uvw_net::sunspec::SunSpecDiscovery;

class AppDiscover {
public:
    AppDiscover();

private:
    DnsServiceDiscovery _dnsDiscovery;
    SunSpecDiscovery _sunspecDiscovery;
    ModbusDiscovery _modbusDiscovery;
};
