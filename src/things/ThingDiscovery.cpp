#include "ThingDiscovery.h"

#include <common/Logger.h>
#include <nlohmann/json.hpp>

#include <uvw/timer.h>
#include <uvw_iot/ThingFactory.h>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/sunspec/SunSpecThing.h>
#include <uvw_net/dns_sd/DnsRecordDataSrv.h>
#include <uvw_net/modbus/ModbusClient.h>

#include <coap/CoapThing.h>
#include <shelly/ShellyGen2Thing.h>

using json = nlohmann::json;
using namespace uvw_iot;
using namespace uvw_iot::sunspec;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;

ThingDiscovery::ThingDiscovery(const ThingRepository& thingRepository) :
    _thingRepository(thingRepository) {

    _coapDiscovery.on<CoapMessage>([this](const CoapMessage& msg, const CoapDiscovery&) {
        const std::string id((char*)msg.token.data(), msg.token.size());
        if (!id.starts_with("PicoW")) return;

        const auto doc = json::from_cbor(msg.payload);      
        if (!doc.is_object() ||
            !doc.count("flap") ||
            !doc.at("flap").is_array() ||
            !(doc.at("flap").size() == 2)) {
            LOG_S(WARNING) << "invalid coap message";
            return;
        }

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

        ThingPropertyMap props;
        props.set<ThingPropertyKey::voltage>(flapValues);

        if (!_thingRepository.things().contains(id)) {
            auto coapThing = CoapThing::from(msg);
            if (coapThing) {
                LOG_S(1) << "add coap device> " << coapThing->id();
                _thingRepository.addThing(coapThing);
            }
        }

        _thingRepository.setThingProperties(id, props);
    });

    _httpDiscovery.on<MdnsResponse>([this](const MdnsResponse& response, const DnsServiceDiscovery&) {
        if (!response.srvData) {
            return;
        }

        const auto& data = *response.srvData;
        LOG_S(1) << "Discovered HTTP service> " << data.target << ":" << data.port;
        const auto host = data.target.substr(0, data.target.find("."));
        auto thing = ThingFactory::from(host);
        if (thing) {
            LOG_S(1) << "add http device> " << thing->id();
            _thingRepository.addThing(thing);
        }
    });

    _shellyDiscovery.on<MdnsResponse>([this](const MdnsResponse& response, const DnsServiceDiscovery&) {
        LOG_S(1) << "Discovered Shelly Gen2 service> "; // << response;
        auto thing = ShellyGen2Thing::from(response);
        if (thing) {
            LOG_S(1) << "add shelly gen2 device> " << thing->id();
            _thingRepository.addThing(thing);
        }
    });

    _sunspecDiscovery.on<SunSpecClientPtr>([this](SunSpecClientPtr client, const SunSpecDiscovery&) {
        LOG_S(INFO) << "add sunspec device> " << client->sunSpecId();
        _thingRepository.addThing(std::make_shared<SunSpecThing>(client));
    });

    _modbusDiscovery.on<ModbusClientPtr>([this](ModbusClientPtr client, const ModbusDiscovery&) {
        _sunspecDiscovery.discover(client);
    });
}

void ThingDiscovery::startDiscovery(int msec) {
    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        LOG_S(1) << "starting discovery cycle";
        _coapDiscovery.discover();
        _httpDiscovery.discover();
        _shellyDiscovery.discover();
        _modbusDiscovery.discover();
    });
    timer->start(uvw::timer_handle::time{2000}, uvw::timer_handle::time{msec});
}
