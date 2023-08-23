#include "SunSpecManager.h"

#include <QDateTime>

#include <common/Logger.h>

using namespace std::placeholders;

namespace sunspec {

SunSpecManager::SunSpecManager(ThingsRepository& repository,
                               QObject *parent) :
    QObject(parent),
    _discovery(repository) {
    _timer.callOnTimeout(this, &SunSpecManager::onTimer);
    _timer.start(100);
}

void SunSpecManager::startDiscovery(uint16_t seconds) {
    _discovery.start(seconds);
}

bool SunSpecManager::contains(const QString& host) const {
    return std::find_if(_things.begin(), _things.end(), [&](const auto* thing_) {
        return thing_->host() == host;
    }) != _things.end();
}

/*
void SunSpecManager::addThing(SunSpecThing* thing) {
    _things.insert(thing->sunSpecId(), thing);
    //connect(thing, &SunSpecThing::stateChanged, this, &SunSpecManager::onThingStateChanged);
    //connect(thing, &SunSpecThing::modelRead, std::bind(&SunSpecManager::modelRead, this, std::ref(*thing), _1, _2));
    //emit thingDiscovered(*thing);
    //addTask({id, cfg->primaryInterval()});
}
*/

void SunSpecManager::addTask(const Task& task) {
    if (!_tasks.contains(task)) {
        _tasks.push_back(task);
    }
}

void SunSpecManager::removeThing(SunSpecThing* thing) {
    if (_things.count(thing->sunSpecId())) {
        LOG_S(WARNING) << "thing removed: " << thing->sunSpecId();
        _things.remove(thing->sunSpecId());
    }
    thing->deleteLater();
}

void SunSpecManager::onTimer() {
    const auto timestamp = (int64_t)std::round(QDateTime::currentMSecsSinceEpoch() / 100.0) * 100;

    // If we have a new day, reset stats
    const auto prev = QDateTime::fromMSecsSinceEpoch(_currentTimestamp);
    const auto now = QDateTime::fromMSecsSinceEpoch(timestamp);
    if (prev.date().day() != now.date().day()) {
        LOG_S(INFO) << "statistics reset";
        emit endOfDayReached();
    }

    // Execute tasks for appropriate timeslots
    foreach (const auto& task, _tasks) {
        if ((timestamp % task.intervalMs.count()) == 0) {
            auto thing = _things.value(task.thing, nullptr);
            if (thing) {
                thing->readModel(task.modelId, timestamp);
            }
        }
    }

    _currentTimestamp = timestamp;
}

/*
void SunSpecManager::onThingStateChanged(SunSpecThing::State state) {
    auto thing = qobject_cast<SunSpecThing*>(sender());
    if (!thing) {
        qFatal("IMPOSSIBLE");
    }
    switch (state) {
    case SunSpecThing::State::Failed:
        // If connection failed, delete thing
        if (_things.count(thing->sunSpecId())) {
            LOG_S(WARNING) << "discovered thing removed: " << thing->sunSpecId();
            _things.remove(thing->sunSpecId());
        }
        break;
    default:
        break;
    }
}
*/

bool SunSpecManager::Task::operator==(const Task& other) {
    return thing == other.thing &&
            modelId == other.modelId &&
            intervalMs == other.intervalMs;
}

} // namespace sunspec
