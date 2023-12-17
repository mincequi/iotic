#include "HttpDiscovery.h"

#include <qmdnsengine/service.h>

#include <common/Logger.h>
#include <things/http/HttpThingFactory.h>

HttpDiscovery::HttpDiscovery(QObject *parent)
    : QObject{parent} {
}

void HttpDiscovery::start(int /*msec*/) {
    stop();
    LOG_S(INFO) << "find things>";
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
        _mdnsBrowser = nullptr;
    }
}

void HttpDiscovery::onServiceAdded(const QMdnsEngine::Service& service) {
    const std::string name = service.name().toStdString();
    LOG_S(INFO) << "thing found> host: " << service.hostname().toStdString() << ", name: " << name;
    auto thing = HttpThingFactory::from({ ThingInfo::DiscoveryType::Http, name, name });
    // TODO: shall we get a subscriber each time, we want to emit (or keep it as class member)?
    if (thing) thingDiscoveredSubscriber().on_next(thing);
}

void HttpDiscovery::onServiceUpdated(const QMdnsEngine::Service&) {
}

void HttpDiscovery::onServiceRemoved(const QMdnsEngine::Service&) {
}
