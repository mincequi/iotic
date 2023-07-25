#include "HttpThing.h"

#include <QNetworkReply>

HttpThing::HttpThing(const ThingInfo& info, QObject *parent)
    : AbstractThing{info, parent} {
    connect(&_nam, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            //onReadBlockError(modelId, reply);
        } else {
            onRead(reply->readAll());
        }
        reply->deleteLater();
    });
}

void HttpThing::read(const std::string& url) {
    _nam.get(QNetworkRequest(QString::fromStdString("http://" + url)));
}
