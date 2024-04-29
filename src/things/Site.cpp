#include "Site.h"

#include <chrono>
#include <numeric>

#include <common/Util.h>
#include <common/Rpp.h>
#include <common/RppUvw.h>
#include <config/Config.h>

#include <uvw_iot/common/ThingRepository.h>

using namespace std::chrono_literals;
using namespace uvw_iot::common;

Site::Site(const ThingRepository& repo, const Config& cfg)
    : _repo(repo),
      _cfg(cfg) {
    _siteDataSubject.get_observable().subscribe([this](const Site::SiteData& data){
        DLOG_S(1) << "{ pv_power: " << data.pvPower
                    << ", grid_power: " << data.gridPower << " }";

        ThingPropertyMap properties;
        properties[ThingPropertyKey::timestamp] = data.ts;
        properties[ThingPropertyKey::pv_power] = data.pvPower;
        properties[ThingPropertyKey::grid_power] = data.gridPower;
        _propertiesSubject.get_observer().on_next(properties);

        while (_history.size() > 3600) {
            _history.pop_front();
        }
        _history.push_back(data);
    });

    _repo.thingAdded().subscribe([this](ThingPtr thing) {
        // If we configured this meter for site (or there is no explicit config)
        if (thing->type() == ThingType::SmartMeter && (_cfg.gridMeter() == thing->id() || _cfg.gridMeter().empty())) {
            LOG_S(INFO) << "thing added> " << thing->id();
            thing->propertiesObservable()
                | filter([&](const auto& p) { return p.count(ThingPropertyKey::power); })
                | map([&](const auto& p) { return std::get<int>(p.at(ThingPropertyKey::power)); })
                | subscribe(_gridPower.get_observer());
        } else if (thing->type() == ThingType::SolarInverter && (_cfg.pvMeters().contains(thing->id()) || _cfg.pvMeters().empty())) {
            LOG_S(INFO) << "thing added> " << thing->id();
            thing->propertiesObservable()
                | filter([thing](const auto& p) { return p.count(ThingPropertyKey::power); })
                | map([thing](const auto& p) { return std::make_pair(thing->id(), std::get<int>(p.at(ThingPropertyKey::power))); })
                | subscribe(_pvPowers.get_observer());
        }
    });

    _pvPowers.get_observable()
        | scan(std::map<std::string, int>{}, [](auto&& result, const auto& value) {
              result[value.first] = value.second;
              return std::move(result);
          })
        | map([](const auto& kv) { return std::accumulate(kv.cbegin(), kv.cend(), 0, [](int result, const auto& value) { return result + value.second; }); })
        | subscribe(_pvPower.get_observer());

    _pvPower.get_observable()
        | combine_latest([](int pvPower, int gridPower) {
        const auto now = std::chrono::system_clock::now();
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return Site::SiteData { (int)seconds, pvPower, gridPower };
    }, _gridPower.get_observable())
    | debounce(std::chrono::milliseconds(_cfg.valueOr<int>("site", Config::Key::debounce, 400)), rpp::schedulers::new_thread{})
    | observe_on(rpp_uvw::schedulers::main_thread_scheduler{})
    | subscribe(_siteDataSubject.get_observer());

    // Init Inverter
    _pvPower.get_observer().on_next(0);

    _mutableProperties[ThingPropertyKey::thing_interval] = _cfg.thingInterval();

    properties().subscribe([this](const ThingPropertyMap& props) {
        for (const auto& kv : props) {
            switch (kv.first) {
            case ThingPropertyKey::thing_interval:
                _cfg.setThingInterval(std::get<int>(kv.second));
                break;
            default:
                break;
            }
        }
    });
}

void Site::setProperty(uvw_iot::common::ThingPropertyKey property, const uvw_iot::common::ThingPropertyValue& value) const {
    // Add property value to local storage for late subscribers
    _mutableProperties[property] = value;

    // Reflect changes back (to other clients as well).
    LOG_S(INFO) << "site.setProperty> " << property << ": " << value;
    _propertiesSubject.get_observer().on_next({{ property, value }});
}

const ThingPropertyMap& Site::mutableProperties() const {
    return _mutableProperties;
}

dynamic_observable<ThingPropertyMap> Site::properties() const {
    return _propertiesSubject.get_observable();
}

dynamic_observable<int> Site::gridPower() const {
    return _gridPower.get_observable();
}

const std::list<Site::SiteData>& Site::history() const {
    return _history;
}
