#pragma once

#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/server.h>

#include <things/ThingsDiscovery.h>

namespace QMdnsEngine {
class Service;
};

class HttpDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    explicit HttpDiscovery(QObject *parent = nullptr);

    void start(int msec) override;
    void stop() override;

private:
    void onServiceAdded(const QMdnsEngine::Service &service);
    void onServiceUpdated(const QMdnsEngine::Service &service);
    void onServiceRemoved(const QMdnsEngine::Service &service);

    QMdnsEngine::Server _mdnsServer;
    QMdnsEngine::Cache _mdnsCache;
    QMdnsEngine::Browser* _mdnsBrowser = nullptr;
};
