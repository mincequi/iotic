#include "Site.h"

#include <numeric>

#include <rpp/operators.hpp>
#include <rpp/observables.hpp>
#include <rpp/rpp.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>
#include <config/Config.h>
#include <things/ThingsRepository.h>

using namespace std::chrono_literals;

Site::Site(const ThingsRepository& thingsRepository) {
    // TODO: this causes other subscribers to lose their subscriptions.
    _gridPower.get_observable().subscribe([](const auto& v) {
        LOG_S(1) << "gridPower: " << v;
    });
    _pvPowers.get_observable().subscribe([](const auto& v) {
        LOG_S(1) << v.first << "> pvPower: " << v.second;
    });

    thingsRepository.thingAdded().subscribe([this](const ThingPtr& thing) {
        // If we configured this meter for site (or there is no explicit config)
        if (thing->type() == Thing::Type::SmartMeter &&
                (cfg->gridMeter() == thing->id() || cfg->gridMeter().empty())) {
            LOG_S(INFO) << "thing added> " << thing->id();
            thing->properties().filter([&](const auto& p) {
                return p.count(ReadableThingProperty::power);
            }).map([&](const auto& p) {
                return (int)p.at(ReadableThingProperty::power);
            }).subscribe(_gridPower.get_subscriber());
        } else if (thing->type() == Thing::Type::SolarInverter &&
                   (cfg->pvMeters().contains(thing->id()) || cfg->pvMeters().empty())) {
            LOG_S(INFO) << "thing added> " << thing->id();
            thing->properties().filter([&](const auto& p) {
                return p.count(ReadableThingProperty::power);
            }).map([&](const auto& p) {
                return std::make_pair(thing->id(), (int)p.at(ReadableThingProperty::power));
            }).subscribe(_pvPowers.get_subscriber());
        }
    });

    _pvPowers.get_observable().scan(std::map<std::string, int>{}, [](auto&& result, const auto& value) {
        result[value.first] = value.second;
        return std::move(result);
    }).map([](const auto& kv) {
        return std::accumulate(kv.cbegin(), kv.cend(), 0, [](int result, const auto& value) { return result + value.second; });
    }).subscribe(_pvPower.get_subscriber());

    _pvPower.get_observable().combine_latest([](int pvPower, int gridPower) {
        return Site::SiteData { pvPower, gridPower, -pvPower - gridPower };
    }, _gridPower.get_observable())
    .debounce(500ms, rpp::schedulers::new_thread{})
    .observe_on(rppqt::schedulers::main_thread_scheduler{})
    .subscribe(_siteData.get_subscriber());
}

rpp::dynamic_observable<Site::SiteData> Site::siteData() const {
    return _siteData.get_observable();
}
