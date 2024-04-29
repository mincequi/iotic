#pragma once

#include <functional>
#include <map>
#include <memory>
#include <things/Site.h>
#include <uvw_iot/common/ThingProperty.h>

namespace uWS {
template <bool SSL, bool isServer, typename WebAppRouter>
struct WebSocket;
}

using uvw_iot::common::ThingPropertyKey;
using uvw_iot::common::ThingPropertyValue;

class WebAppRouter : public std::enable_shared_from_this<WebAppRouter> {
public:
    WebAppRouter(const ThingRepository& thingRepository,
                 const Site& site,
                 const Config& cfg);

    inline const ThingRepository& thingRepository() const { return _thingRepository; }
    inline const Site& site() const { return _site; }
    inline const Config& cfg() const { return _cfg; }

    bool route(const std::string& thing, ThingPropertyKey property, const ThingPropertyValue& value);

private:
    std::vector<uint8_t> serializeSiteHistory(const std::list<Site::SiteData>& history, uint from, uint to);
    void onBinaryMessage(uWS::WebSocket<false, true, std::shared_ptr<WebAppRouter>>* ws, std::string_view message);

    std::map<std::pair<std::string, ThingPropertyKey>, std::function<void(const ThingPropertyValue&)>> _routes;

    const ThingRepository& _thingRepository;
    const Site& _site;
    const Config& _cfg;

    friend class WebAppBehavior;
    friend class WebAppRouterTest;
};

using WebAppRouterPtr = std::shared_ptr<WebAppRouter>;
