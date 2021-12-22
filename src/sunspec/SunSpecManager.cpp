#include "SunSpecManager.h"

#include <QDateTime>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>

#include "../Logger.h"

using namespace std::placeholders;

namespace sunspec {

SunSpecManager::SunSpecManager(QObject *parent) : QObject(parent) {
    m_timer.callOnTimeout(this, &SunSpecManager::onTimer);
    m_timer.start(1000);

    m_discoverTimer.callOnTimeout(this, &SunSpecManager::onStartDiscovering);
}

void SunSpecManager::startDiscovering(uint16_t seconds) {
    onStartDiscovering();
    m_discoverTimer.start(seconds * 1000);
}

void SunSpecManager::stopDiscovering() {
    m_discoverTimer.stop();
}

void SunSpecManager::onStartDiscovering() {
    foreach (auto thing, m_discoveringThings) {
        thing->deleteLater();
    }
    m_discoveringThings.clear();

    // Find network interfaces
    QString subnet;
    const QHostAddress localhost = QHostAddress(QHostAddress::LocalHost);
    foreach (const auto& address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
             subnet = address.toString().left(address.toString().lastIndexOf(".")+1);
        }
    }

    // Scan subnets
    QTcpSocket socket;
    LOG_S(INFO) << "discovering things in subnet: " << subnet << "0/24";
    for (uint8_t i = 1; i < 255; ++i) {
        const QString host = subnet + QString::number(i);
        if (std::find_if(m_discoveredThings.begin(),
                         m_discoveredThings.end(),
                         [&](sunspec::SunSpecThing* _thing) { return _thing->host() == host; }) != m_discoveredThings.end()) {
            continue;
        }
        auto* thing = new sunspec::SunSpecThing(host);
        m_discoveringThings.append(thing);
        thing->connect(thing, &sunspec::SunSpecThing::stateChanged, this, &SunSpecManager::onThingStateChanged);
        thing->connectDevice();
    }
}

void SunSpecManager::addTask(const Task& task) {
    if (!m_tasks.contains(task)) {
        m_tasks.push_back(task);
    }
}

void SunSpecManager::removeThing(SunSpecThing* thing) {
    m_discoveringThings.removeOne(thing);
    if (m_discoveredThings.count(thing->sunSpecId())) {
        LOG_S(WARNING) << "discovered thing removed: " << thing->sunSpecId();
        m_discoveredThings.remove(thing->sunSpecId());
    }
    thing->deleteLater();
}

void SunSpecManager::onThingStateChanged(SunSpecThing::State state) {
    auto thing = qobject_cast<SunSpecThing*>(sender());
    if (!thing) {
        qFatal("IMPOSSIBLE");
    }
    switch (state) {
    case SunSpecThing::State::Failed:
        removeThing(thing);
        break;
    case SunSpecThing::State::Connected:
        m_discoveringThings.removeAll(thing);
        m_discoveredThings.insert(thing->sunSpecId(), thing);
        connect(thing, &SunSpecThing::modelRead, std::bind(&SunSpecManager::modelRead, this, std::ref(*thing), _1, _2));
        emit thingDiscovered(*thing);
        break;
    }
}

void SunSpecManager::onTimer() {
    const auto timestamp = QDateTime::currentSecsSinceEpoch();

    // If we have a new day, reset stats
    const auto prev = QDateTime::fromSecsSinceEpoch(m_currentTimestamp);
    const auto now = QDateTime::fromSecsSinceEpoch(timestamp);
    if (prev.date().day() != now.date().day()) {
        LOG_S(INFO) << "statistics reset";
        emit endOfDayReached();
    }

    // Execute tasks for appropriate timeslots
    foreach (const auto& task, m_tasks) {
        if ((timestamp % task.interval) == 0) {
            auto thing = m_discoveredThings.value(task.thing, nullptr);
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
            interval == other.interval;
}

} // namespace sunspec
