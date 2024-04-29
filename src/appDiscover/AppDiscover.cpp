#include "AppDiscover.h"

#include <iostream>

#include <uvw_iot/common/ThingFactory.h>
#include <uvw_net/dns_sd/DnsRecordDataSrv.h>
#include <uvw_net/dns_sd/DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

#include <uvw/loop.h>
#include <uvw/timer.h>

using namespace std::chrono_literals;
using namespace uvw_net;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;
using namespace uvw_net::sunspec;

AppDiscover::AppDiscover() {
    _dnsDiscovery.on<DnsRecordDataSrv>([&](const DnsRecordDataSrv& data, const DnsServiceDiscovery&) {
        const auto host = data.target.substr(0, data.target.find("."));
        std::cout << "http service found> host: " << host << ", port: " << data.port << std::endl;
    });

    _sunspecDiscovery.on<SunSpecClientPtr>([&](SunSpecClientPtr client, const SunSpecDiscovery&) {
        std::stringstream supportedModels;
        for (const auto& kv : client->models()) {
            auto m = magic_enum::enum_cast<SunSpecModel::Id>(kv.first);
            if (m.has_value())
                supportedModels << magic_enum::enum_name(m.value())  << " (" << kv.first << "), ";
        }
        std::cout << "sunspec thing found> id: " << client->sunSpecId()
                  << ", host: " << client->host()
                  << ", unitId: " << (uint32_t)client->unitId()
                  << ", supportedModels: " << supportedModels.str() << std::endl;
    });

    _modbusDiscovery.on<ModbusClientPtr>([&](ModbusClientPtr client, const ModbusDiscovery&) {
        _sunspecDiscovery.discover(client);
    });

    // Start a discovery timer
    auto discoveryTimer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    discoveryTimer->on<uvw::timer_event>([&](const auto&, auto&) {
        std::cout << "start discoveries>";
        _dnsDiscovery.discover("_http._tcp.local");
        _modbusDiscovery.discover();
    });
    discoveryTimer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{30000});
}

