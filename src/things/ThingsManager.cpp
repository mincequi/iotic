#include "ThingsManager.h"

#include <cmath>

#include <uvw/timer.h>
#include <uvw_iot/ThingRepository.h>

#include <config/ConfigRepository.h>

ThingsManager::ThingsManager(const ThingRepository& thingRepository, const ConfigRepository& cfg) :
    _thingRepository(thingRepository),
    _configRepository(cfg) {

    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        onTimer();
    });
    timer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{100});
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
    if ((now_ms % (_configRepository.thingInterval() * 1000)) == 0) {
        _thingRepository.fetchProperties();
    }
}
