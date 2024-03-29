#include "WebAppRouter.h"

#include <App.h> // This is uws
#include <nlohmann/json.hpp>

#include <common/Logger.h>
#include <common/Util.h>
#include <config/Config.h>
#include <things/Site.h>
#include <things/ThingId.h>
#include <things/ThingProperty.h>

using namespace std::placeholders;
using json = nlohmann::json;

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

std::vector<uint8_t> WebAppRouter::serializeSiteHistory(const std::list<Site::SiteData>& history, uint from, uint to) {
    const auto itb = history.begin();
    const auto ite = history.end();
    auto itl = std::lower_bound(itb, ite, from, [](const Site::SiteData& data, uint value) {
        return data.ts <= value;
    });
    const auto itu = std::upper_bound(itb, ite, to, [](uint value, const Site::SiteData& data) {
        return value <= data.ts;
    });
    if (itl != itb) --itl;
    if (itl == ite || std::distance(itl, itu) < 1) {
        LOG_S(WARNING) << "invalid requested range: [" << from << ".." << to << "], history: [" << itb->ts << ".." << history.back().ts << "]";
        return {};
    }

    std::vector<uint> timestamps;
    std::vector<int> pvPowers;
    std::vector<int> gridPowers;
    for (auto it = itl; it != itu; ++it) {
        timestamps.push_back(it->ts);
        pvPowers.push_back(it->pvPower);
        gridPowers.push_back(it->gridPower);
    }
    for (auto& v : gridPowers) v /= 10;
    for (auto& v : pvPowers) v /= 10;
    for (auto it = gridPowers.rbegin(); it != gridPowers.rend(); ++it) {
        if (it+1 != gridPowers.rend()) {
            *it -= *(it+1);
        }
    }
    for (auto it = pvPowers.rbegin(); it != pvPowers.rend(); ++it) {
        if (it+1 != pvPowers.rend()) {
            *it -= *(it+1);
        }
    }
    for (auto it = timestamps.rbegin(); it != timestamps.rend(); ++it) {
        if (it+1 != timestamps.rend()) {
            *it -= *(it+1);
        }
    }
    json j;
    j[util::toString(ThingId::site)][util::toString(Property::timestamp)] = timestamps;
    j[util::toString(ThingId::site)][util::toString(Property::pv_power)] = pvPowers;
    j[util::toString(ThingId::site)][util::toString(Property::grid_power)] = gridPowers;

    return json::to_cbor(j);
}

void WebAppRouter::onBinaryMessage(uWS::WebSocket<false, true, WebAppRouter>* ws, std::string_view message) {
    const auto doc = json::from_cbor(message);
    if (doc.size() != 1) {
        LOG_S(WARNING) << "unknown binary message with size: " << message.size();
        return;
    }

    const auto& kv = doc.items().begin();
    if (kv.key() != util::toString(ThingId::site)) {
        LOG_S(WARNING) << "unknown thing: " << kv.key();
        return;
    }

    if (!kv.value().contains(util::toString(Property::timestamp))) {
        LOG_S(WARNING) << "timestamp not found";
        return;
    }

    const auto timestamps = kv.value().at(util::toString(Property::timestamp)).get<std::vector<uint>>();
    if (timestamps.size() != 2) {
        LOG_S(WARNING) << "invalid timestamps length";
        return;
    }

    const auto buffer = serializeSiteHistory(_site->history(), timestamps.front(), timestamps.back());
    if (!buffer.empty()) {
        ws->send(std::string_view(reinterpret_cast<const char*>(buffer.data()), buffer.size()));
    }
}
