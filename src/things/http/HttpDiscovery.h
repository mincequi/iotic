#pragma once

#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/server.h>

#include <rpp/fwd.hpp>
#include <rpp/observables.hpp>
#include <rpp/sources.hpp>
#include <rpp/subjects/publish_subject.hpp>

#include <things/Thing.h>
#include <things/ThingsDiscovery.h>

namespace QMdnsEngine {
class Service;
};

using rpp::subjects::publish_subject;

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
