#include "WebSocketExporter.h"

#include <QtWebSockets>
#include <common/Util.h>
#include <things/ThingsRepository.h>

WebSocketExporter::WebSocketExporter(const ThingsRepository& thingsRepository,
                                     QObject *parent) :
    QObject(parent),
    _thingsRepository(thingsRepository),
    _server(new QWebSocketServer(QStringLiteral("iotic"), QWebSocketServer::NonSecureMode, this)) {
    if (_server->listen(QHostAddress::Any, 80)) {
        connect(_server, &QWebSocketServer::newConnection, this, &WebSocketExporter::onNewConnection);
        connect(_server, &QWebSocketServer::closed, this, &WebSocketExporter::closed);

        _thingsRepository.site().siteData().subscribe([this](const Site::SiteData& data) {
            QJsonObject site;
            site["pvPower"] = data.pvPower;
            site["gridPower"] = data.gridPower;
            site["sitePower"] = data.sitePower;
            QJsonObject json;
            json["site"] = site;

            for (const auto &c : std::as_const(_clients)) {
                c->sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact));
            }
        });

        _thingsRepository.thingAdded().subscribe([this](const ThingPtr& thing) {
            thing->properties().subscribe([this, &thing](const std::map<ReadableThingProperty, double>& prop) {
                QJsonObject thing_;
                if (thing->type() != Thing::Type::Undefined)
                    thing_["type"] = QString::fromStdString(util::toString(thing->type()));
                if (!thing->name().empty())
                    thing_["name"] = QString::fromStdString(thing->name());
                if (thing->icon())
                    thing_["icon"] = thing->icon();
                for (const auto& kv : prop) {
                    thing_[QString::fromStdString(util::toString(kv.first))] = kv.second;
                }
                QJsonObject json;
                json[QString::fromStdString(thing->id())] = thing_;

                for (const auto &c : std::as_const(_clients)) {
                    c->sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact));
                }
            });
        });
    }
}

WebSocketExporter::~WebSocketExporter() {
    _server->close();
    qDeleteAll(_clients.begin(), _clients.end());
}

void WebSocketExporter::onNewConnection() {
    QWebSocket* client = _server->nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &WebSocketExporter::onMessageReceived);
    //connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketExporter::processBinaryMessage);
    connect(client, &QWebSocket::disconnected, this, &WebSocketExporter::onSocketDisconnected);

    _clients << client;
}

void WebSocketExporter::onMessageReceived(const QString& message) {
    QWebSocket* client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        client->sendTextMessage(message);
    }
}

void WebSocketExporter::onSocketDisconnected() {
    QWebSocket* client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        _clients.removeAll(client);
        client->deleteLater();
    }
}
