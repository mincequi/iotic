#include "HttpDiscovery.h"

#include <QDateTime>
#include <qmdnsengine/service.h>

#include <common/Logger.h>

HttpDiscovery::HttpDiscovery(ThingManager& thingManager, QObject *parent)
    : AbstractDiscovery(thingManager, parent) {
}

void HttpDiscovery::start() {
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
    onThingDiscovered({ ThingInfo::Type::Http, service.name().toStdString() });
}

void HttpDiscovery::onServiceUpdated(const QMdnsEngine::Service &service) {
}

void HttpDiscovery::onServiceRemoved(const QMdnsEngine::Service &service) {
}
