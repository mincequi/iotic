#include "HttpDiscovery.h"

#include <QDateTime>
#include <qmdnsengine/service.h>

#include <common/Logger.h>
#include <rpp/operators/map.hpp>
#include <rpp/operators.hpp>
#include <rpp/sources.hpp>

HttpDiscovery::HttpDiscovery(QObject *parent)
    : QObject{parent} {
}

void HttpDiscovery::start(int /*msec*/) {
    stop();
    LOG_S(INFO) << "discovering things";
    _mdnsBrowser = new QMdnsEngine::Browser(&_mdnsServer, "_http._tcp.local.", &_mdnsCache);
    QObject::connect(_mdnsBrowser, &QMdnsEngine::Browser::serviceAdded,
                     this, &HttpDiscovery::onServiceAdded);
    QObject::connect(_mdnsBrowser, &QMdnsEngine::Browser::serviceUpdated,
                     this, &HttpDiscovery::onServiceUpdated);
    QObject::connect(_mdnsBrowser, &QMdnsEngine::Browser::serviceRemoved,
                     this, &HttpDiscovery::onServiceRemoved);
}

void HttpDiscovery::stop() {
    if (_mdnsBrowser) {
        _mdnsBrowser->disconnect();
        _mdnsBrowser->deleteLater();
    }
}

void HttpDiscovery::onServiceAdded(const QMdnsEngine::Service &service) {
    const std::string name = service.name().toStdString();
    LOG_S(INFO) << "thing discovered> host: " << service.hostname().toStdString() << ", name: " << name;
    // TODO: shall we get a subscriber each time, we want to emit (or keep it as class member)?
    _thingDiscovered.get_subscriber().on_next({ ThingInfo::DiscoveryType::Http, name, name });
}

void HttpDiscovery::onServiceUpdated(const QMdnsEngine::Service &service) {
}

void HttpDiscovery::onServiceRemoved(const QMdnsEngine::Service &service) {
}
