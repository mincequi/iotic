#pragma once

#include <things/Thing.h>

class QNetworkAccessManager;
class QNetworkReply;

class HttpThing : public Thing {
public:
    explicit HttpThing(const ThingInfo& info);
    virtual ~HttpThing();

protected:
    void read(const std::string& url);
    void write(const std::string& url);
    virtual void onRead(const QByteArray& reply) = 0;

    QNetworkAccessManager* _nam;
    QNetworkReply* _pendingReply = nullptr;
};
