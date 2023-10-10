#include "HttpThing.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <common/Logger.h>

HttpThing::HttpThing(const ThingInfo& info)
    : Thing{info},
      _nam(new QNetworkAccessManager) {
    _nam->setTransferTimeout(5000);
    _nam->connect(_nam, &QNetworkAccessManager::finished, [this](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            //_properties.get_subscriber().on_error(std::exception_ptr{});
            LOG_S(WARNING) << id() << "> error: " << reply->errorString().toStdString();
            reply->deleteLater();
            _pendingReply = nullptr;
            _stateSubject.get_subscriber().on_next(State::Failed);
        } else if (_pendingReply == reply) {
            onRead(reply->readAll());
            reply->deleteLater();
            _pendingReply = nullptr;
        }
    });
}

HttpThing::~HttpThing() {
    _nam->disconnect();
    _nam->deleteLater();
}

void HttpThing::read(const std::string& url) {
    _pendingReply = _nam->get(QNetworkRequest(QString::fromStdString("http://" + url)));
}

void HttpThing::write(const std::string& url) {
    _nam->get(QNetworkRequest(QString::fromStdString("http://" + url)));
}
