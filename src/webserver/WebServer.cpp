#include "WebServer.h"

#include <App.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmrc/cmrc.hpp>
#include <uvw/loop.h>

#include <common/Logger.h>
#include <common/Util.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

#include "OcppBehavior.h"
#include "WebAppBehavior.h"

CMRC_DECLARE(webapp);

WebServer::WebServer(const ThingsRepository& thingsRepository) :
    _thingsRepository(thingsRepository) {
    _fs = std::make_unique<cmrc::embedded_filesystem>(cmrc::webapp::get_filesystem());
    uWS::Loop::get(uvw::loop::get_default()->raw());
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
    _uwsApp->ws<WebAppRouter>("/ws", WebAppBehavior::create());
    //_uwsApp->ws<UserData>("/ocpp", OcppBehavior::create());
    _uwsApp->ws<UserData>("/ocpp/*", OcppBehavior::create());
    _uwsApp->listen(8030, [](auto* listenSocket) {
        if (!listenSocket) {
            LOG_S(FATAL) << "port 8030 already in use";
        }
    });

    _site->properties().subscribe([this](const auto& props) {
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

    cfg->timeConstant().get_observable().subscribe([this](int value) {
        QJsonObject properties;
        properties[QString::fromStdString(util::toString(MutableProperty::time_constant))] = QJsonValue(value);

        QJsonObject json;
        json["ev_charging_strategy"] = properties;

        _uwsApp->publish("broadcast",
                         QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact).toStdString(),
                         uWS::OpCode::TEXT);
    });

    _thingsRepository.thingAdded().subscribe([this](const ThingPtr& thing) {
        // When new thing is added, send persistent properties to each client
        _uwsApp->publish("broadcast",
                         WebAppBehavior::serializeUserProperties(thing),
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
