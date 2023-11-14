#include "Site.h"

#include <chrono>
#include <numeric>

#include <rpp/operators.hpp>
#include <rpp/observables.hpp>
#include <rpp/rpp.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>

#include <common/Logger.h>
#include <config/Config.h>
#include <things/ThingId.h>
#include <things/ThingsRepository.h>

using namespace std::chrono_literals;

Site* Site::instance() {
    if (!_instance) _instance = new Site();
    return _instance;
}

Site::Site() {
    _siteDataSubject.get_observable().subscribe([this](const Site::SiteData& data){
        LOG_S(INFO) << "{ pv_power: " << data.pvPower
                    << ", grid_power: " << data.gridPower
                    << ", site_power: " << data.sitePower << " }";

        std::map<Property, Value> properties;
        properties[Property::timestamp] = data.ts;
        properties[Property::pv_power] = (double)data.pvPower;
        properties[Property::grid_power] = (double)data.gridPower;
        properties[Property::site_power] = (double)data.sitePower;
        _propertiesSubject.get_subscriber().on_next(properties);

        while (_history.size() > 122) {
            _history.pop_front();
        }
        _history.push_back(data);

        repo->update();
    });

    repo->thingAdded().subscribe([this](const ThingPtr& thing) {
        // If we configured this meter for site (or there is no explicit config)
        if (thing->type() == Thing::Type::SmartMeter &&
                (cfg->gridMeter() == thing->id() || cfg->gridMeter().empty())) {
            LOG_S(INFO) << "thing added> " << thing->id();
            thing->properties().filter([&](const auto& p) {
                return p.count(Property::power);
            }).map([&](const auto& p) {
                return (int)std::get<double>(p.at(Property::power));
            }).subscribe(_gridPower.get_subscriber());
        } else if (thing->type() == Thing::Type::SolarInverter &&
                   (cfg->pvMeters().contains(thing->id()) || cfg->pvMeters().empty())) {
            LOG_S(INFO) << "thing added> " << thing->id();
            thing->properties().filter([&](const auto& p) {
                return p.count(Property::power);
            }).map([&](const auto& p) {
                return std::make_pair(thing->id(), (int)std::get<double>(p.at(Property::power)));
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
        const auto now = std::chrono::system_clock::now();
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return Site::SiteData { (int)seconds, pvPower, gridPower, -pvPower - gridPower };
    }, _gridPower.get_observable())
    .debounce(std::chrono::milliseconds(cfg->valueOr<int>("site", Config::Key::debounce, 400)), rpp::schedulers::new_thread{})
    .observe_on(rppqt::schedulers::main_thread_scheduler{})
    .subscribe(_siteDataSubject.get_subscriber());

    // Init Inverter
    _pvPower.get_subscriber().on_next(0);

    _mutableProperties[MutableProperty::thing_interval] = cfg->thingInterval();

    properties().subscribe([this](const std::map<Property, Value>& props) {
        for (const auto& kv : props) {
            switch (kv.first) {
            case Property::thing_interval:
                cfg->setThingInterval(std::get<double>(kv.second));
                break;
            default:
                break;
            }
        }
    });
}

void Site::setProperty(MutableProperty property, const Value& value) const {
    // Add property value to local storage for late subscribers
    _mutableProperties[property] = value;

    // Reflect changes back (to other clients as well).
    const auto property_ = magic_enum::enum_cast<Property>(magic_enum::enum_integer(property));
    LOG_IF_S(FATAL, !property_.has_value()) << util::toString(property) << " has no readable correspondent";
    LOG_S(INFO) << "site.setProperty> " << util::toString(property_.value()) << ": " << value;
    _propertiesSubject.get_subscriber().on_next({{ property_.value(), value }});
}

const std::map<MutableProperty, Value>& Site::mutableProperties() const {
    return _mutableProperties;
}

dynamic_observable<std::map<Property, Value>> Site::properties() const {
    return _propertiesSubject.get_observable();
}

const std::list<Site::SiteData>& Site::history() const {
    return _history;
}
