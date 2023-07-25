#pragma once

#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/server.h>

#include <iot/AbstractDiscovery.h>
#include <iot/AbstractThing.h>

namespace QMdnsEngine {
class Service;
};

class HttpDiscovery : public AbstractDiscovery {
    Q_OBJECT
public:
    explicit HttpDiscovery(ThingManager& thingManager, QObject *parent = nullptr);

    void start() override;
    void stop() override;

private:
    void onServiceAdded(const QMdnsEngine::Service &service);
    void onServiceUpdated(const QMdnsEngine::Service &service);
    void onServiceRemoved(const QMdnsEngine::Service &service);

    QMdnsEngine::Server _mdnsServer;
    QMdnsEngine::Cache _mdnsCache;
    std::unique_ptr<QMdnsEngine::Browser> _mdnsBrowser;
};
