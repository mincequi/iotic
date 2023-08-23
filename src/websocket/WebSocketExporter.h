#pragma once

#include <QObject>

class QWebSocket;
class QWebSocketServer;
class ThingsRepository;

class WebSocketExporter : public QObject {
    Q_OBJECT
public:
    explicit WebSocketExporter(const ThingsRepository& thingsRepository, QObject *parent = nullptr);
    ~WebSocketExporter();

signals:
    void closed();

private slots:
    void onNewConnection();
    void onMessageReceived(const QString& message);
    void onSocketDisconnected();

private:
    const ThingsRepository& _thingsRepository;
    QWebSocketServer* _server;
    QList<QWebSocket*> _clients;
};
