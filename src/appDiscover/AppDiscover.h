#pragma once

#include <uvw_net/dns_sd//DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

#include <coap/CoapDiscovery.h>

using uvw_net::dns_sd::DnsServiceDiscovery;
using uvw_net::modbus::ModbusDiscovery;
using uvw_net::sunspec::SunSpecDiscovery;

class AppDiscover {
public:
    AppDiscover();

private:
    CoapDiscovery _coapDiscovery;
    DnsServiceDiscovery _httpDiscovery{"_http._tcp.local."};
    DnsServiceDiscovery _shellyGen2PlusDiscovery{"_shelly._tcp.local."};
    SunSpecDiscovery _sunspecDiscovery;
    ModbusDiscovery _modbusDiscovery;
};
