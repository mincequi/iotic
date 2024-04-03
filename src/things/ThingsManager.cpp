#include "ThingsManager.h"

#include <uvw/timer.h>

#include <config/Config.h>
#include <common/Logger.h>
#include <fronius/FroniusDiscovery.h>
#include <modbus/ModbusDiscovery.h>
#include <sunspec/SunSpecDiscovery.h>
#include <things/ThingsRepository.h>
#include <things/http/HttpDiscovery.h>
#include <things/http/HttpThingFactory.h>

ThingsManager::ThingsManager(CandidatesRepository& candidatesRepository,
                             ThingsRepository& thingsRepository)
    : _candidatesRepository(candidatesRepository),
      _thingsRepository(thingsRepository) {

    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        onTimer();
    });
    timer->start(uvw::timer_handle::time{100}, uvw::timer_handle::time{100});

    _discoveries.push_back(std::make_shared<HttpDiscovery>());
    auto modbusDiscovery = std::make_shared<ModbusDiscovery>();
    auto sunSpecDiscovery = std::make_shared<SunSpecDiscovery>(modbusDiscovery);
    _discoveries.push_back(sunSpecDiscovery);
    _discoveries.push_back(std::make_shared<FroniusDiscovery>(sunSpecDiscovery));

    for (const auto& d : _discoveries) {
        d->thingDiscovered().subscribe([this](ThingPtr thing) {
            LOG_S(2) << "add thing> id: " << thing->id();
            _thingsRepository.addThing(std::move(thing));
        });
    }

    // Add ModbusDiscovery later, since we do not want to subscribe to thingDiscovered() here.
    _discoveries.push_back(modbusDiscovery);
}

void ThingsManager::startDiscovery(int msec) {
    onDiscoveryTimer();

    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        onDiscoveryTimer();
    });
    timer->start(uvw::timer_handle::time{msec}, uvw::timer_handle::time{msec});
}

void ThingsManager::stopDiscovery() {
    for (const auto& d : _discoveries) {
        d->stop();
    }
}

void ThingsManager::onTimer() {
    //const auto timestamp = (int64_t)std::round(QDateTime::currentMSecsSinceEpoch() / 100.0) * 100;

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
    if ((now_ms % (cfg->thingInterval() * 1000)) == 0) {
        for (const auto& t : _thingsRepository.things()) {
            t->read();
        }
    }

    //_currentTimestamp = timestamp;
}

void ThingsManager::onDiscoveryTimer() {
    for (const auto& d : _discoveries) {
        d->start(60000);
    }
}
