#pragma once

#include <QNetworkAccessManager>

#include <things/Thing.h>

class HttpThing : public QObject, public Thing {
    Q_OBJECT
public:
    explicit HttpThing(const ThingInfo& info, QObject *parent = nullptr);

protected:
    void read(const std::string& url);
    void write(const std::string& url);
    virtual void onRead(const QByteArray& reply) = 0;

    QNetworkAccessManager _nam;
    QNetworkReply* _pendingReplay = nullptr;
};
