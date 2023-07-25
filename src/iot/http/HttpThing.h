#pragma once

#include <QNetworkAccessManager>

#include <iot/AbstractThing.h>

class HttpThing : public AbstractThing {
public:
    explicit HttpThing(const ThingInfo& info, QObject *parent = nullptr);

protected:
    void read(const std::string& url);
    virtual void onRead(const QByteArray& reply) = 0;

    QNetworkAccessManager _nam;
};
