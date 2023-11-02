#include "WebServer.h"

#include <App.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmrc/cmrc.hpp>

#include <common/Logger.h>
#include <common/Util.h>
#include <things/ThingsRepository.h>

CMRC_DECLARE(webapp);

WebServer::WebServer(void* mainLoop,
                     const ThingsRepository& thingsRepository) :
    _thingsRepository(thingsRepository) {
    _fs = std::make_unique<cmrc::embedded_filesystem>(cmrc::webapp::get_filesystem());
    uWS::Loop::get(mainLoop);
    struct UserData {};
    uWS::App::WebSocketBehavior<UserData> behavior;
    behavior.open = [this](uWS::WebSocket<false, true, UserData>* ws) {
        ws->subscribe("broadcast");

        // Send all things and their persistent properties to new client.
        for (const auto& t : _thingsRepository.things()) {
            ws->send(serializeUserProperties(t), uWS::OpCode::TEXT);
        }

        // Send historic site data to new client
        QJsonArray timestamps;
        QJsonArray pvPower;
        QJsonArray sitePower;
        QJsonArray gridPower;
        for (const auto& p : _thingsRepository.site().history()) {
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

        ws->send(QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact).toStdString(), uWS::OpCode::TEXT);
    };
    behavior.message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
        //ws->send(message, opCode);
        const auto obj = QJsonDocument::fromJson({message.data(), (int)message.size()}).object();
        if (obj.isEmpty()) return;

        const auto thingId = obj.begin().key().toStdString();
        const auto property = magic_enum::enum_cast<MutableProperty>(obj.begin().value().toObject().begin().key().toStdString());
        const QJsonValue value = obj.begin().value().toObject().begin().value();

        if (property)
            _thingsRepository.setThingProperty(thingId, property.value(), fromQJsonValue(value));
    };

    _uwsApp = std::make_unique<uWS::App>();
    _uwsApp->get("/", [this](uWS::HttpResponse<false>* res, auto* req) {
        auto f = _fs->open("index.html");
        res->end(std::string_view(f.begin(), f.end()-f.begin()));
    });
    _uwsApp->get("/*", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        const auto fileName = std::string(req->getUrl());
        LOG_S(INFO) << "get> " << fileName;

        if (!_fs->exists(fileName)) {
            LOG_S(ERROR) << "file not found: " << fileName;
            res->end("file not found: " + fileName);
            return;
        }
        auto f = _fs->open(fileName);
        res->end(std::string_view(f.begin(), f.end()-f.begin()));
    });
    _uwsApp->ws<UserData>("/ws", std::move(behavior)).listen(8030, [](auto* listenSocket) {
        if (!listenSocket) {
            LOG_S(FATAL) << "port 8030 already in use";
        }
    });

    _thingsRepository.site().properties().subscribe([this](const auto& props) {
        QJsonObject siteProperties;
        for (const auto& p : props) {
            siteProperties[QString::fromStdString(util::toString(p.first))] = toJsonValue(p.second);
        }

        QJsonObject json;
        json["site"] = siteProperties;

        _uwsApp->publish("broadcast",
                         QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact).toStdString(),
                         uWS::OpCode::TEXT);
    });

    _thingsRepository.thingAdded().subscribe([this](const ThingPtr& thing) {
        // When new thing is added, send persistent properties to each client
        _uwsApp->publish("broadcast",
                         serializeUserProperties(thing),
                         uWS::OpCode::TEXT);

        thing->properties().subscribe([this, &thing](const std::map<Property, Value>& prop) {
            QJsonObject thing_;
            for (const auto& kv : prop) {
                if (kv.first <= Property::power_control)
                    thing_[QString::fromStdString(util::toString(kv.first))] = toJsonValue(kv.second);
            }
            QJsonObject json;
            json[QString::fromStdString(thing->id())] = thing_;

            _uwsApp->publish("broadcast",
                             QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact).toStdString(),
                             uWS::OpCode::TEXT);
        });
    });
}

WebServer::~WebServer() {
}

std::string WebServer::serializeUserProperties(const ThingPtr& t) {
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
