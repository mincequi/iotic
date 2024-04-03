#include "WebAppBehavior.h"

#include <nlohmann/json.hpp>

// Note: this has to be included for magic enum
#include <common/Logger.h>
#include <common/Util.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

using json = nlohmann::json;

uWS::App::WebSocketBehavior<WebAppRouter> WebAppBehavior::create() {
    uWS::App::WebSocketBehavior<WebAppRouter> behavior;
    behavior.open = [](auto* ws) {
        ws->subscribe("broadcast");

        // Send all things and their persistent properties to new client.
        for (const auto& t : repo->things()) {
            ws->send(serializeUserProperties(t), uWS::OpCode::TEXT);
        }

        ws->send(serializeSiteProperties(*_site), uWS::OpCode::TEXT);
        ws->send(serializeEvChargingStrategyProperties(*cfg), uWS::OpCode::TEXT);
        ws->send(serializeSiteHistory(_site->history()), uWS::OpCode::TEXT);
    };
    behavior.message = [](uWS::WebSocket<false, true, WebAppRouter>* ws, std::string_view message, uWS::OpCode opCode) {
        switch (opCode) {
        case uWS::BINARY:
            return ws->getUserData()->onBinaryMessage(ws, message);
        default:
            break;
        }

        const auto doc = json::parse(message);
        if (!doc.is_object() || doc.empty()) return;
        const auto thingId = doc.begin().key();
        const auto pv = doc.begin().value();
        if (!pv.is_object() || pv.empty()) return;
        const auto property = magic_enum::enum_cast<MutableProperty>(pv.begin().key());
        const auto value = pv.begin().value();

        if (property) {
            // Check if our router can route this (site and ev_charging_strategy)
            // Otherwise, set property to generic repo.
            if (!ws->getUserData()->route(thingId, property.value(), fromJsonValue(value))) {
                repo->setThingProperty(thingId, property.value(), fromJsonValue(value));
            }
        }
    };

    return behavior;
}

std::string WebAppBehavior::serializeUserProperties(const ThingPtr& t) {
    json properties;
    for (const auto& kv : t->mutableProperties()) {
        if (kv.first <= MutableProperty::power_control)
            properties[util::toString(kv.first)] = toJsonValue(kv.second);
    }
    if (t->type() != Thing::Type::Undefined)
        properties["type"] = util::toString(t->type());

    json thing;
    thing[t->id()] = properties;
    return thing.dump();
}

std::string WebAppBehavior::serializeSiteProperties(const Site& site) {
    json properties;
    for (const auto& kv : site.mutableProperties()) {
        if (kv.first <= MutableProperty::power_control)
            properties[util::toString(kv.first)] = toJsonValue(kv.second);
    }

    json thing;
    thing["site"] = properties;
    return thing.dump();
}

std::string WebAppBehavior::serializeSiteHistory(const std::list<Site::SiteData>& siteHistory) {
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
    siteProperties[util::toString(Property::timestamp)] = timestamps;
    siteProperties[util::toString(Property::pv_power)] = pvPower;
    siteProperties[util::toString(Property::grid_power)] = gridPower;
    json json;
    json["site"] = siteProperties;
    return json.dump();
}

std::string WebAppBehavior::serializeEvChargingStrategyProperties(const Config& config) {
    json properties;
    properties[util::toString(MutableProperty::time_constant)] = config.timeConstant().get_value();

    json thing;
    thing["ev_charging_strategy"] = properties;
    return thing.dump();
}
