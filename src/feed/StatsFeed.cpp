#include "StatsFeed.h"

#include "LiveFeed.h"

#include <rpp/rpp.hpp>

StatsFeed::StatsFeed(const LiveFeed& liveFeed)
    : _maxSitePowerSubject(0),
      _maxSummedInvertersPowerSubject(0) {
    liveFeed.combinedPowers().map([this](const std::tuple<int, int>& val) {
        // compute consumption
        return std::max(std::get<0>(val) - std::get<1>(val), _maxSitePowerSubject.get_value());
        //}).scan(0, [](int&& seed, int newVal) {
        //    seed = std::max(seed, newVal);
        //    return std::move(seed);
    }).subscribe(_maxSitePowerSubject.get_subscriber());

    liveFeed.combinedPowers().map([this](const std::tuple<int, int>& val) {
        // compute consumption
        return std::max(std::get<0>(val), _maxSummedInvertersPowerSubject.get_value());
    }).subscribe(_maxSummedInvertersPowerSubject.get_subscriber());
}

void StatsFeed::reset() const {
    _maxSitePowerSubject.get_subscriber().on_next(0);
    _maxSummedInvertersPowerSubject.get_subscriber().on_next(0);
}

rpp::dynamic_observable<int> StatsFeed::maxSummedInverterPower() const {
    return _maxSummedInvertersPowerSubject.get_observable();
}

rpp::dynamic_observable<int> StatsFeed::maxSitePower() const {
    return _maxSitePowerSubject.get_observable();
}

rpp::dynamic_observable<int> StatsFeed::maxPower() const {
    return _maxSummedInvertersPowerSubject.get_observable()
            .combine_latest([](int left, int right) { return std::max(left, right); },
                _maxSitePowerSubject.get_observable()
            );
}
