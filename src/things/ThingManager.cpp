#include "ThingManager.h"

#include <QDateTime>
#include <config/Config.h>
#include <common/Logger.h>
#include <things/ThingFactory.h>
#include <things/http/HttpDiscovery.h>

ThingManager::ThingManager(QObject *parent)
    : QObject{parent} {
    _timer.callOnTimeout(this, &ThingManager::onTimer);
    _timer.start(100);

    _discoveries.push_back(std::make_unique<HttpDiscovery>());

    for (const auto& d : _discoveries) {
        d->thingDiscovered().subscribe([this](const ThingInfo& t) {
            LOG_S(INFO) << "thing discovered: " << t.id();
            auto thing = ThingFactory::from(t);
            if (thing) {
                addThing(std::move(thing));
            }
        });
    }
}

void ThingManager::startDiscovery() {
    for (const auto& d : _discoveries) {
        d->start();
    }
}

void ThingManager::stopDiscovery() {
    for (const auto& d : _discoveries) {
        d->stop();
    }
}

void ThingManager::addTask(const Task& task) {
    if (!std::count(_tasks.begin(), _tasks.end(), task)) {
        _tasks.push_back(task);
    }
}

void ThingManager::addThing(ThingPtr thing) {
    if (_things.count(thing->id())) {
        LOG_S(INFO) << "already added thing: " << thing->id();
        return;
    }
    const auto id = thing->id();
    LOG_S(INFO) << "added thing: " << id;
    _things[id] = std::move(thing);
    addTask({id, cfg->primaryInterval()});
}

void ThingManager::onTimer() {
    const auto timestamp = (int64_t)std::round(QDateTime::currentMSecsSinceEpoch() / 100.0) * 100;

    // If we have a new day, reset stats
    const auto prev = QDateTime::fromMSecsSinceEpoch(_currentTimestamp);
    const auto now = QDateTime::fromMSecsSinceEpoch(timestamp);
    if (prev.date().day() != now.date().day()) {
        LOG_S(INFO) << "statistics reset";
        emit endOfDayReached();
    }

    // Execute tasks for appropriate timeslots
    for (const auto& task : _tasks) {
        if ((timestamp % task.intervalMs.count()) == 0) {
            if (_things.count(task.thingId)) {
                _things.at(task.thingId)->read();
            }
        }
    }

    _currentTimestamp = timestamp;
}


bool ThingManager::Task::operator==(const Task& other) {
    return thingId == other.thingId &&
            intervalMs == other.intervalMs;
}
