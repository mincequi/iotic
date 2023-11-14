#include "ThingsManager.h"

#include <QDateTime>
#include <config/Config.h>
#include <common/Logger.h>
#include <things/ThingFactory.h>
#include <things/ThingsRepository.h>
#include <things/http/HttpDiscovery.h>

ThingsManager::ThingsManager(ThingsRepository& thingsRepository, QObject *parent)
    : QObject{parent},
      _thingsRepository(thingsRepository) {
    _timer.callOnTimeout(this, &ThingsManager::onTimer);
    _timer.start(100);
    _discoveryTimer.callOnTimeout(this, &ThingsManager::onDiscoveryTimer);

    _discoveries.push_back(std::make_unique<HttpDiscovery>());

    for (const auto& d : _discoveries) {
        d->thingDiscovered().subscribe([this](const ThingInfo& t) {
            auto thing = ThingFactory::from(t);
            if (thing) {
                _thingsRepository.addThing(std::move(thing));
            }
        });
    }
}

void ThingsManager::startDiscovery(int msec) {
    onDiscoveryTimer();
    _discoveryTimer.start(msec);
}

void ThingsManager::stopDiscovery() {
    for (const auto& d : _discoveries) {
        d->stop();
    }
}

void ThingsManager::onTimer() {
    const auto timestamp = (int64_t)std::round(QDateTime::currentMSecsSinceEpoch() / 100.0) * 100;

    // If we have a new day, reset stats
    const auto prev = QDateTime::fromMSecsSinceEpoch(_currentTimestamp);
    const auto now = QDateTime::fromMSecsSinceEpoch(timestamp);
    if (prev.date().day() != now.date().day()) {
        LOG_S(INFO) << "statistics reset";
        emit endOfDayReached();
    }

    // Read things
    if ((timestamp % (cfg->thingInterval() * 1000)) == 0) {
        for (const auto& t : _thingsRepository.things()) {
            t->read();
        }
    }

    _currentTimestamp = timestamp;
}

void ThingsManager::onDiscoveryTimer() {
    for (const auto& d : _discoveries) {
        d->start(60000);
    }
}
