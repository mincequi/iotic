#pragma once

#include <QObject>

#include <things/Thing.h>

class QHttpServer;
class QWebSocket;
class QWebSocketServer;
class ThingsRepository;

namespace cmrc {
class embedded_filesystem;
}

class WebServer : public QObject {
    Q_OBJECT
public:
    explicit WebServer(const ThingsRepository& thingsRepository, QObject *parent = nullptr);
    ~WebServer();

private slots:
    void onNewConnection();
    void onMessageReceived(const QString& message) const;
    void onSocketDisconnected();

private:
    void initWebSocketServer();
    void initWebServer();

    static QByteArray serializePersistentProperties(const ThingPtr& thing);

    const ThingsRepository& _thingsRepository;

    // TODO: Qt does not seem to work with a single QHttpServer serving HTTP and WS.
    //       It also does not work, when creating two instances of QHttpServer
    //       with one serving HTTP and the other serving WS.
    //       However it works, if a single instance serves HTTP OR WS!
    //       Also works if QHttpServer serves HTTP and QWebSocketServer serves WS!
    QHttpServer* _httpServer;
    //std::vector<std::unique_ptr<QWebSocket>> _clients;
    QWebSocketServer* _wsServer;
    QList<QWebSocket*> _clients;

    std::unique_ptr<cmrc::embedded_filesystem> _fs;
};
