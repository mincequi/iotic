#include "WebAppBehavior.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// Note: this has to be included for magic enum
#include <common/Logger.h>
#include <common/Util.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

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
    behavior.message = [](auto* ws, std::string_view message, uWS::OpCode) {
        //ws->send(message, opCode);
        const auto obj = QJsonDocument::fromJson({message.data(), (int)message.size()}).object();
        if (obj.isEmpty()) return;

        const auto thingId = obj.begin().key().toStdString();
        const auto property = magic_enum::enum_cast<MutableProperty>(obj.begin().value().toObject().begin().key().toStdString());
        const QJsonValue value = obj.begin().value().toObject().begin().value();

        if (property) {
            if (!ws->getUserData()->route(thingId, property.value(), fromQJsonValue(value))) {
                repo->setThingProperty(thingId, property.value(), fromQJsonValue(value));
            }
        }
    };

    return behavior;
}

std::string WebAppBehavior::serializeUserProperties(const ThingPtr& t) {
    QJsonObject properties;
    for (const auto& kv : t->mutableProperties()) {
        if (kv.first <= MutableProperty::power_control)
            properties[QString::fromStdString(util::toString(kv.first))] = toJsonValue(kv.second);
    }
    if (t->type() != Thing::Type::Undefined)
        properties["type"] = QString::fromStdString(util::toString(t->type()));

    QJsonObject thing;
    thing[QString::fromStdString(t->id())] = properties;
    return QJsonDocument(thing).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
}

std::string WebAppBehavior::serializeSiteProperties(const Site& site) {
    QJsonObject properties;
    for (const auto& kv : site.mutableProperties()) {
        if (kv.first <= MutableProperty::power_control)
            properties[QString::fromStdString(util::toString(kv.first))] = toJsonValue(kv.second);
    }

    QJsonObject thing;
    thing["site"] = properties;
    return QJsonDocument(thing).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
}

std::string WebAppBehavior::serializeSiteHistory(const std::list<Site::SiteData>& siteHistory) {
    // Send historic site data to new client
    QJsonArray timestamps;
    QJsonArray pvPower;
    QJsonArray sitePower;
    QJsonArray gridPower;
    for (const auto& p : siteHistory) {
        timestamps.append(p.ts);
        pvPower.append(p.pvPower);
        sitePower.append(p.sitePower);
        gridPower.append(p.gridPower);
    }

    QJsonObject siteProperties;
    siteProperties.insert(QString::fromStdString(util::toString(Property::timestamp)), timestamps);
    siteProperties.insert(QString::fromStdString(util::toString(Property::pv_power)), pvPower);
    siteProperties.insert(QString::fromStdString(util::toString(Property::site_power)), sitePower);
    siteProperties.insert(QString::fromStdString(util::toString(Property::grid_power)), gridPower);
    QJsonObject json;
    json["site"] = siteProperties;
    return QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
}

std::string WebAppBehavior::serializeEvChargingStrategyProperties(const Config& config) {
    QJsonObject properties;
    properties[QString::fromStdString(util::toString(MutableProperty::time_constant))] = QJsonValue(config.timeConstant().get_value());

    QJsonObject thing;
    thing["ev_charging_strategy"] = properties;
    return QJsonDocument(thing).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
}
