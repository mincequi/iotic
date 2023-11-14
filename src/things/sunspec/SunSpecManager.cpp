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

void SunSpecManager::startDiscovery(int msec) {
    _discovery.start(msec);
}

bool SunSpecManager::contains(const QString& host) const {
    return std::find_if(_things.begin(), _things.end(), [&](const auto* thing_) {
        return thing_->host() == host;
    }) != _things.end();
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

    _currentTimestamp = timestamp;
}

} // namespace sunspec
