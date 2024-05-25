#include "WebAppBehavior.h"

#include <nlohmann/json.hpp>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/util/Site.h>

#include <common/Logger.h>  // Note: this has to be included for magic enum
#include <common/Util.h>
#include <config/Config.h>
#include <things/ThingValue.h>

using namespace uvw_iot;
using json = nlohmann::json;

uWS::App::WebSocketBehavior<WebAppRouterPtr> WebAppBehavior::create(WebAppRouterPtr router) {
    uWS::App::WebSocketBehavior<WebAppRouterPtr> behavior;
    behavior.open = [router](uWS::WebSocket<false, true, WebAppRouterPtr>* ws) mutable {
        WebAppRouterPtr* userData = ws->getUserData();
        *userData = router;
        ws->subscribe("broadcast");

        // Send all things and their persistent properties to new client.
        for (const auto& t : router->thingRepository().things()) {
            ws->send(serializeUserProperties(t.second), uWS::OpCode::TEXT);
        }

        ws->send(serializeSiteProperties(router->cfg()), uWS::OpCode::TEXT);
        ws->send(serializeEvChargingStrategyProperties(router->cfg()), uWS::OpCode::TEXT);
        ws->send(serializeSiteHistory(router->site().history()), uWS::OpCode::TEXT);
    };
    behavior.close = [](uWS::WebSocket<false, true, WebAppRouterPtr>* ws, int, std::string_view) {
        ws->unsubscribe("broadcast");
    };
    behavior.message = [](uWS::WebSocket<false, true, WebAppRouterPtr>* ws, std::string_view message, uWS::OpCode opCode) {
        WebAppRouterPtr router = *ws->getUserData();
        switch (opCode) {
        case uWS::BINARY:
            return router->onBinaryMessage(ws, message);
        default:
            break;
        }

        const auto doc = json::parse(message);
        if (!doc.is_object() || doc.empty()) return;
        const auto thingId = doc.begin().key();
        const auto pv = doc.begin().value();
        if (!pv.is_object() || pv.empty()) return;
        const auto property = magic_enum::enum_cast<ThingPropertyKey>(pv.begin().key());
        const auto value = pv.begin().value();

        if (property) {
            // Check if our router can route this (site and ev_charging_strategy)
            // Otherwise, set property to generic repo.
            if (!router->route(thingId, property.value(), fromJsonValue(value))) {
                router->thingRepository().setThingProperty(thingId, property.value(), fromJsonValue(value));
            }
        }
    };

    return behavior;
}

std::string WebAppBehavior::serializeUserProperties(const ThingPtr& t) {
    json properties;
    for (const auto& kv : t->properties()) {
        if (kv.first <= ThingPropertyKey::power_control)
            properties[::util::toString(kv.first)] = toJsonValue(kv.second);
    }
    if (t->type() != ThingType::Unknown)
        properties["type"] = ::util::toString(t->type());

    json thing;
    thing[t->id()] = properties;
    return thing.dump();
}

std::string WebAppBehavior::serializeSiteHistory(const std::list<Site::Properties>& siteHistory) {
    // Send historic site data to new client
    json timestamps = json::array();
    json pvPower = json::array();
    json gridPower = json::array();
    for (auto it = siteHistory.rbegin(); it != siteHistory.rend() && timestamps.size() <= 120; ++it) {
        timestamps.insert(timestamps.begin(), it->ts);
        pvPower.insert(pvPower.begin(), it->pvPower);
        gridPower.insert(gridPower.begin(), it->gridPower);
    }

    json siteProperties;
    siteProperties[::util::toString(ThingPropertyKey::timestamp)] = timestamps;
    siteProperties[::util::toString(ThingPropertyKey::pv_power)] = pvPower;
    siteProperties[::util::toString(ThingPropertyKey::grid_power)] = gridPower;
    json json;
    json["site"] = siteProperties;
    return json.dump();
}

std::string WebAppBehavior::serializeSiteProperties(const Config& config) {
    json properties;
    properties[::util::toString(ThingPropertyKey::thing_interval)] = config.thingInterval();

    json thing;
    thing["site"] = properties;
    return thing.dump();
}

std::string WebAppBehavior::serializeEvChargingStrategyProperties(const Config& config) {
    json properties;
    properties[::util::toString(ThingPropertyKey::time_constant)] = config.timeConstant();

    json thing;
    thing["ev_charging_strategy"] = properties;
    return thing.dump();
}
