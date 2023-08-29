#include "WebSocketExporter.h"

#include <QHttpServer>
#include <QtWebSockets>
#include <cmrc/cmrc.hpp>
#include <common/Util.h>
#include <things/ThingsRepository.h>

CMRC_DECLARE(webapp);

WebSocketExporter::WebSocketExporter(const ThingsRepository& thingsRepository,
                                     QObject *parent) :
    QObject(parent),
    _thingsRepository(thingsRepository),
    _httpServer(new QHttpServer(this)),
    _wsServer(new QWebSocketServer(QStringLiteral("iotic"), QWebSocketServer::NonSecureMode, this)) {
    //_wsServer(new QHttpServer()) {

    _fs = std::make_unique<cmrc::embedded_filesystem>(cmrc::webapp::get_filesystem());

    _httpServer->route("/", [this]() {
        auto f = _fs->open("index.html");
        return QHttpServerResponse(f.begin());
    });
    _httpServer->route("/<arg>", [this](const QUrl& url) {
        auto f = _fs->open(url.path().toStdString());
        return QHttpServerResponse(QByteArray(f.begin(), f.end()-f.begin()));
    });
    auto httpPort = _httpServer->listen(QHostAddress::Any, 7090);

    // We need to create a route for websockets, which does not respond anything.
    //_httpServer->route("/ws", [this](const QString &arg, const QHttpServerRequest&) {
      //qDebug() << __func__ << request  << arg;
      //return QFuture<QHttpServerResponse>();
    //});
    //_wsServer->route("/ws", [](QHttpServerResponder&&) {});

    if (_wsServer->listen(QHostAddress::Any, httpPort+1)) {
        connect(_wsServer, &QWebSocketServer::newConnection, this, &WebSocketExporter::onNewConnection);

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
    _wsServer->close();
    qDeleteAll(_clients.begin(), _clients.end());
    //_clients.clear();
}

void WebSocketExporter::onNewConnection() {
    auto client = _wsServer->nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &WebSocketExporter::onMessageReceived);
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
        /*for (auto it = _clients.begin(); it != _clients.end(); ++it) {
            if (it->get() == client) {
                _clients.erase(it);
                return;
            }
        }*/
    }
}
