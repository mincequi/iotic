#include "DiscoveryManager.h"

#include <common/Logger.h>
#include <nlohmann/json.hpp>

#include <uvw/timer.h>
#include <uvw_iot/ThingFactory.h>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/sunspec/SunSpecThing.h>
#include <uvw_net/dns_sd/DnsRecordDataSrv.h>
#include <uvw_net/modbus/ModbusClient.h>

#include <coap/CoapThing.h>

using json = nlohmann::json;
using namespace uvw_iot;
using namespace uvw_iot::sunspec;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;

DiscoveryManager::DiscoveryManager(const ThingRepository& thingRepository) :
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

    _dnsDiscovery.on<DnsRecordDataSrv>([this](const DnsRecordDataSrv& data, const DnsServiceDiscovery&) {
        const auto host = data.target.substr(0, data.target.find("."));
        auto thing = ThingFactory::from(host);
        if (thing) {
            LOG_S(1) << "add http device> " << thing->id();
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

void DiscoveryManager::startDiscovery(int msec) {
    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        _dnsDiscovery.discover("_http._tcp.local");
        _modbusDiscovery.discover();
    });
    timer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{msec});
}
