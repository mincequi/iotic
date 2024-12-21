#pragma once

#include <App.h>
#include <uvw_iot/Thing.h>
#include <uvw_iot/util/Site.h>

#include "WebAppRouter.h"

class Config;
namespace uvw_iot {
class ThingRepository;
}

using uvw_iot::ThingPtr;

class WebAppBehavior {
public:
    WebAppBehavior() = delete;
    static uWS::App::WebSocketBehavior<WebAppRouterPtr> create(WebAppRouterPtr router);

    static std::string serializeUserProperties(const ThingPtr& thing);
    static std::string serializeSiteProperties(const Config& config);
    static std::string serializeSiteHistory(const std::list<Site::Properties>& siteHistory);
    static std::string serializeEvChargingStrategyProperties(const Config& config);
};
