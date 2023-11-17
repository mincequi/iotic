#pragma once

#include <uvw/loop.h>
#include <QTimer>
#include <things/Site.h>
#include <things/Thing.h>
#include <webserver/Router.h>

class Config;
class ThingsRepository;

namespace cmrc {
class embedded_filesystem;
}

namespace uWS {
template <bool SSL>
struct TemplatedApp;
typedef TemplatedApp<false> App;
}

class WebServer {
public:
    explicit WebServer(const ThingsRepository& thingsRepository);
    ~WebServer();

private:
    static std::string serializeUserProperties(const ThingPtr& thing);
    static std::string serializeSiteProperties(const Site& site);
    static std::string serializeSiteHistory(const std::list<Site::SiteData>& siteHistory);
    static std::string serializeEvChargingStrategyProperties(const Config& config);

    const ThingsRepository& _thingsRepository;
    std::unique_ptr<cmrc::embedded_filesystem> _fs;
    std::shared_ptr<uvw::loop> _loop = uvw::loop::get_default();
    std::unique_ptr<uWS::App> _uwsApp;

    Router _router;
    QTimer _timer;
};
