#include "HttpDiscovery.h"

#include <QDateTime>
#include <qmdnsengine/service.h>

#include <common/Logger.h>
#include <rpp/operators/map.hpp>
#include <rpp/operators.hpp>
#include <rpp/sources.hpp>
#include <rppqt/sources.hpp>

HttpDiscovery::HttpDiscovery(QObject *parent)
    : QObject{parent} {
}

void HttpDiscovery::start(uint16_t seconds) {
    _mdnsBrowser = std::make_unique<QMdnsEngine::Browser>(&_mdnsServer, "_http._tcp.local.", &_mdnsCache);
    QObject::connect(_mdnsBrowser.get(), &QMdnsEngine::Browser::serviceAdded,
                     this, &HttpDiscovery::onServiceAdded);
    QObject::connect(_mdnsBrowser.get(), &QMdnsEngine::Browser::serviceUpdated,
                     this, &HttpDiscovery::onServiceUpdated);
    QObject::connect(_mdnsBrowser.get(), &QMdnsEngine::Browser::serviceRemoved,
                     this, &HttpDiscovery::onServiceRemoved);
}

void HttpDiscovery::stop() {
    _mdnsBrowser.reset();
}

void HttpDiscovery::onServiceAdded(const QMdnsEngine::Service &service) {
    // TODO: shall we get a subscriber each time, we want to emit (or keep it as class member)?
    _thingDiscovered.get_subscriber().on_next({ ThingInfo::Type::Http, service.name().toStdString() });
}

void HttpDiscovery::onServiceUpdated(const QMdnsEngine::Service &service) {
}

void HttpDiscovery::onServiceRemoved(const QMdnsEngine::Service &service) {
}
