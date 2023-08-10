#include "SunSpecManager.h"

#include <QDateTime>

#include <common/Logger.h>

using namespace std::placeholders;

namespace sunspec {

SunSpecManager::SunSpecManager(QObject *parent) :
    QObject(parent),
    _discovery(*this) {
    m_timer.callOnTimeout(this, &SunSpecManager::onTimer);
    m_timer.start(100);
}

void SunSpecManager::startDiscovery(uint16_t seconds) {
    _discovery.start(seconds);
}

bool SunSpecManager::contains(const QString& host) const {
    return std::find_if(
                _things.begin(), _things.end(),
                [&](const auto* thing_) {
                     return thing_->host() == host;
                }) != _things.end();
}

void SunSpecManager::addThing(SunSpecThing* thing) {
    _things.insert(thing->sunSpecId(), thing);
    connect(thing, &SunSpecThing::modelRead, std::bind(&SunSpecManager::modelRead, this, std::ref(*thing), _1, _2));
    emit thingDiscovered(*thing);

}

void SunSpecManager::addTask(const Task& task) {
    if (!m_tasks.contains(task)) {
        m_tasks.push_back(task);
    }
}

void SunSpecManager::removeThing(SunSpecThing* thing) {
    _discovery.removeThing(thing);
    if (_things.count(thing->sunSpecId())) {
        LOG_S(WARNING) << "discovered thing removed: " << thing->sunSpecId();
        _things.remove(thing->sunSpecId());
    }
    thing->deleteLater();
}

void SunSpecManager::onTimer() {
    const auto timestamp = (int64_t)std::round(QDateTime::currentMSecsSinceEpoch() / 100.0) * 100;

    // If we have a new day, reset stats
    const auto prev = QDateTime::fromMSecsSinceEpoch(m_currentTimestamp);
    const auto now = QDateTime::fromMSecsSinceEpoch(timestamp);
    if (prev.date().day() != now.date().day()) {
        LOG_S(INFO) << "statistics reset";
        emit endOfDayReached();
    }

    // Execute tasks for appropriate timeslots
    foreach (const auto& task, m_tasks) {
        if ((timestamp % task.intervalMs.count()) == 0) {
            auto thing = _things.value(task.thing, nullptr);
            if (thing) {
                thing->readModel(task.modelId, timestamp);
            }
        }
    }

    m_currentTimestamp = timestamp;
}

bool SunSpecManager::Task::operator==(const Task& other) {
    return thing == other.thing &&
            type == other.type &&
            modelId == other.modelId &&
            intervalMs == other.intervalMs;
}

} // namespace sunspec
