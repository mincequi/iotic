#include "HttpThing.h"

#include <QNetworkReply>

HttpThing::HttpThing(const ThingInfo& info, QObject *parent)
    : QObject(parent),
      Thing{info} {
    connect(&_nam, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            _properties.get_subscriber().on_error(std::exception_ptr{});
        } else {
            onRead(reply->readAll());
        }
        reply->deleteLater();
    });
}

void HttpThing::read(const std::string& url) {
    _nam.get(QNetworkRequest(QString::fromStdString("http://" + url)));
}