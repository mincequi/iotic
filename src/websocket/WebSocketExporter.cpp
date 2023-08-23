#include "WebSocketExporter.h"

#include <QtWebSockets>
#include <things/ThingsRepository.h>

WebSocketExporter::WebSocketExporter(const ThingsRepository& thingsRepository,
                                     QObject *parent) :
    QObject(parent),
    _thingsRepository(thingsRepository),
    _server(new QWebSocketServer(QStringLiteral("iotic"), QWebSocketServer::NonSecureMode, this)) {
    if (_server->listen(QHostAddress::Any, 80)) {
        connect(_server, &QWebSocketServer::newConnection, this, &WebSocketExporter::onNewConnection);
        connect(_server, &QWebSocketServer::closed, this, &WebSocketExporter::closed);

        _thingsRepository.site().siteData().subscribe([](const Site::SiteData& data){
            LOG_S(INFO) << "site> { pvPower: " << data.pvPower
                        << ", gridPower: " << data.gridPower
                        << ", sitePower: " << data.sitePower << " }";
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
