#include "ThingsManager.h"

#include <cmath>

#include <uvw/timer.h>
#include <uvw_iot/ThingFactory.h>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/sunspec/SunSpecThing.h>
#include <uvw_net/dns_sd/DnsRecordDataSrv.h>

#include <common/Logger.h>
#include <config/Config.h>
#include <fronius/FroniusDiscovery.h>

using namespace uvw_iot;
using namespace uvw_iot::sunspec;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;

ThingsManager::ThingsManager(const ThingRepository& thingRepository, const Config& cfg) :
    _thingRepository(thingRepository),
    _cfg(cfg) {

    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        onTimer();
    });
    timer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{100});

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

void ThingsManager::startDiscovery(int msec) {
    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        _dnsDiscovery.discover("_http._tcp.local");
        //_modbusDiscovery.discover();
    });
    timer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{msec});
}

void ThingsManager::discover() {
    //_dnsDiscovery.discover("_http._tcp.local");
    _modbusDiscovery.discover();
}

void ThingsManager::onTimer() const {
    const auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    now_ms = (int64_t)std::round(now_ms / 100.0) * 100;

    // If we have a new day, reset stats
    //const auto prev = QDateTime::fromMSecsSinceEpoch(_currentTimestamp);
    //const auto now = QDateTime::fromMSecsSinceEpoch(timestamp);
    //if (prev.date().day() != now.date().day()) {
    //    LOG_S(1) << "statistics reset";
    //    emit endOfDayReached();
    //}

    // Read things
    if ((now_ms % (_cfg.thingInterval() * 1000)) == 0) {
        _thingRepository.getProperties();
    }

    //_currentTimestamp = timestamp;
}
