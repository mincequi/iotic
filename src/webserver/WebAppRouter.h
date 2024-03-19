#pragma once

#include <functional>
#include <map>
#include <things/Site.h>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

namespace uWS {
template <bool SSL, bool isServer, typename WebAppRouter>
struct WebSocket;
}

class WebAppRouter {
public:
    WebAppRouter();

    bool route(const std::string& thing, MutableProperty property, const Value& value);

private:
    std::vector<uint8_t> serializeSiteHistory(const std::list<Site::SiteData>& history, uint from, uint to);
    void onBinaryMessage(uWS::WebSocket<false, true, WebAppRouter>* ws, std::string_view message);

    std::map<std::pair<std::string, MutableProperty>, std::function<void(const Value&)>> _routes;

    friend class WebAppBehavior;
    friend class WebAppRouterTest;
};
