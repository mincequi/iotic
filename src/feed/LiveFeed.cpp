#include "LiveFeed.h"

#include <numeric>
#include <rpp/rpp.hpp>

using namespace std::chrono_literals;

LiveFeed::LiveFeed() {
    _invertersSubject
            .get_observable()
            // Create a group of observables discriminated by key-string
            .group_by([](const std::pair<std::string, int>& pair) { return pair.first; },
                      [](const std::pair<std::string, int>& pair) { return pair.second; })
            // Add timeout to each observable
            .flat_map([](const rpp::grouped_observable_group_by<std::string, int>& obs) {
                return obs
                        .timeout(10s, rpp::source::just(0), rpp::schedulers::new_thread{})
                        .map([k = obs.get_key()](int v) {
                            return std::pair{k, v};
                        })
                        .repeat();
            })
            .scan(std::map<std::string, int>{},
                  [](std::map<std::string, int>&& seed,
                  std::pair<std::string, int> newPair) {
                        seed[newPair.first] = newPair.second;
                        return std::move(seed);
                  })
            .map([](const std::map<std::string, int>& keyValue) {
                return std::accumulate(keyValue.cbegin(), keyValue.cend(),
                                       0,
                                       [](int val, const auto& pair) {
                                            return val + pair.second;
                                        });
            })
            .subscribe(_summedInverterSubject.get_subscriber());

    _summedInverterSubject.get_observable()
            .combine_latest([](int inverterSum, int meter) {
                return std::make_tuple(inverterSum, meter);
            }, _meterSubject.get_observable())
            .distinct_until_changed()
            .debounce(300ms, rpp::schedulers::current_thread{})
            .subscribe(_combinedPowers.get_subscriber());
}

void LiveFeed::setInverterPower(const std::string& id, int power) const {
    _invertersSubject.get_subscriber().on_next({id, power});
}

void LiveFeed::setGridMeterPower(const std::string& id, int power) const {
    // TODO: filter for meter
    _meterSubject.get_subscriber().on_next(power);
}

rpp::dynamic_observable<std::tuple<int, int>> LiveFeed::combinedPowers() const {
    return _combinedPowers.get_observable();
}

rpp::dynamic_observable<int> LiveFeed::summedInverterPower() const {
    return _summedInverterSubject.get_observable();
}

rpp::dynamic_observable<int> LiveFeed::meterPower() const {
    return _meterSubject.get_observable();
}
