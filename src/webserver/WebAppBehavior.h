#pragma once

#include <App.h>

#include <things/Site.h>
#include <things/Thing.h>

#include "WebAppRouter.h"

class Config;

class WebAppBehavior {
public:
    WebAppBehavior() = delete;
    static uWS::App::WebSocketBehavior<WebAppRouter> create();

    static std::string serializeUserProperties(const ThingPtr& thing);
    static std::string serializeSiteProperties(const Site& site);
    static std::string serializeSiteHistory(const std::list<Site::SiteData>& siteHistory);
    static std::string serializeEvChargingStrategyProperties(const Config& config);
};
