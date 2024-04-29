#pragma once

#include <App.h>
#include <uvw_iot/common/Thing.h>

#include <things/Site.h>

#include "WebAppRouter.h"

class Config;
namespace uvw_iot::common {
class ThingRepository;
}

using uvw_iot::common::ThingPtr;

class WebAppBehavior {
public:
    WebAppBehavior() = delete;
    static uWS::App::WebSocketBehavior<WebAppRouterPtr> create(WebAppRouterPtr router);

    static std::string serializeUserProperties(const ThingPtr& thing);
    static std::string serializeSiteProperties(const Site& site);
    static std::string serializeSiteHistory(const std::list<Site::SiteData>& siteHistory);
    static std::string serializeEvChargingStrategyProperties(const Config& config);
};
