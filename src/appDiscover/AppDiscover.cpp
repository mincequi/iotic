#include "AppDiscover.h"

#include <iostream>

#include <nlohmann/json.hpp>

#include <uvw_iot/ThingFactory.h>
#include <uvw_net/dns_sd/DnsRecordDataSrv.h>
#include <uvw_net/dns_sd/DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

#include <uvw/loop.h>
#include <uvw/timer.h>

#include <common/Logger.h>

using namespace std::chrono_literals;
using namespace uvw_net;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;
using namespace uvw_net::sunspec;

using json = nlohmann::json;

AppDiscover::AppDiscover() {
    _coapDiscovery.on<CoapMessage>([this](const CoapMessage& msg, const CoapDiscovery&) {
        if (msg.code != CoapMessage::Code::Content) return;

        const auto doc = json::from_cbor(msg.payload);
        if (!doc.is_object() ||
            !doc.count("flap") ||
            !doc.at("flap").is_array() ||
            !(doc.at("flap").size() == 2)) {
            LOG_S(WARNING) << "invalid coap message";
            return;
        }

        LOG_S(INFO) << "new coap message> " << doc;

        const float _referenceVoltage = 3.3f;
        const float _maxValue = 4095.f;
        const float _voltageDividerCorrection = 3.2f;
        float flap1 = doc.at("flap").at(0).get<int>();
        float flap2 = doc.at("flap").at(1).get<int>();
        flap1 = flap1 * _referenceVoltage / _maxValue * _voltageDividerCorrection;
        flap2 = flap2 * _referenceVoltage / _maxValue * _voltageDividerCorrection;

        // Scale to percentage
        flap1 = (flap1 - 2.0f) / 8.0f * 100.0f;
        flap2 = (flap2 - 2.0f) / 8.0f * 100.0f;

        std::array<int, 3> flapValues;
        flapValues[0] = roundf(flap1);
        flapValues[1] = roundf(flap2);
        flapValues[2] = 0;

        LOG_S(INFO) << "flap values> " << flapValues[0] << ", " << flapValues[1];
    });

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
        std::cout << "start discoveries>" << std::endl;
        _dnsDiscovery.discover("_http._tcp.local");
        _modbusDiscovery.discover();
    });
    discoveryTimer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{30000});
}

