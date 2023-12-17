#include "WebAppRouter.h"

#include <config/Config.h>
#include <things/Site.h>

using namespace std::placeholders;

WebAppRouter::WebAppRouter() {
    _routes[{"site", MutableProperty::thing_interval}] =
            std::bind(&Site::setProperty, _site, MutableProperty::thing_interval, _1);
    _routes[{"ev_charging_strategy", MutableProperty::time_constant}] =
            std::bind(&Config::setTimeConstant, cfg, _1);
}

bool WebAppRouter::route(const std::string& thing, MutableProperty property, const Value& value) {
    if (!_routes.contains({thing, property})) return false;

    _routes.at({thing, property})(value);
    return true;
}
