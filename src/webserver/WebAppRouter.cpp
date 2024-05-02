#include "WebAppRouter.h"

#include <App.h> // This is uws
#include <nlohmann/json.hpp>
#include <uvw_iot/Site.h>
#include <uvw_iot/ThingProperty.h>

#include <common/Logger.h>
#include <common/Util.h>
#include <config/Config.h>
#include <things/ThingId.h>

using namespace std::placeholders;
using namespace uvw_iot;
using json = nlohmann::json;

WebAppRouter::WebAppRouter(const ThingRepository& repo, const Site& site, const Config& cfg)
    : _thingRepository(repo),
    _site(site),
    _cfg(cfg) {
    // TODO: fix thing interval setting
    _routes[{"site", ThingPropertyKey::thing_interval}] =
        std::bind(&Config::setThingInterval, &_cfg, _1);
    _routes[{"ev_charging_strategy", ThingPropertyKey::time_constant}] =
        std::bind(&Config::setTimeConstant, &_cfg, _1);
}

bool WebAppRouter::route(const std::string& thing, ThingPropertyKey property, const ThingPropertyValue& value) {
    if (!_routes.contains({thing, property})) return false;

    _routes.at({thing, property})(value);
    return true;
}

std::vector<uint8_t> WebAppRouter::serializeSiteHistory(const std::list<Site::Properties>& history, int from, int to) {
    const auto itb = history.begin();
    const auto ite = history.end();
    auto itl = std::lower_bound(itb, ite, from, [](const Site::Properties& data, uint value) {
        return data.ts <= value;
    });
    const auto itu = std::upper_bound(itb, ite, to, [](uint value, const Site::Properties& data) {
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
    j[util::toString(ThingId::site)][util::toString(ThingPropertyKey::timestamp)] = timestamps;
    j[util::toString(ThingId::site)][util::toString(ThingPropertyKey::pv_power)] = pvPowers;
    j[util::toString(ThingId::site)][util::toString(ThingPropertyKey::grid_power)] = gridPowers;

    return json::to_cbor(j);
}

void WebAppRouter::onBinaryMessage(uWS::WebSocket<false, true, std::shared_ptr<WebAppRouter>>* ws, std::string_view message) {
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

    if (!kv.value().contains(util::toString(ThingPropertyKey::timestamp))) {
        LOG_S(WARNING) << "timestamp not found";
        return;
    }

    const auto timestamps = kv.value().at(util::toString(ThingPropertyKey::timestamp)).get<std::vector<int>>();
    if (timestamps.size() != 2) {
        LOG_S(WARNING) << "invalid timestamps length";
        return;
    }

    const auto buffer = serializeSiteHistory(_site.history(), timestamps.front(), timestamps.back());
    if (!buffer.empty()) {
        ws->send(std::string_view(reinterpret_cast<const char*>(buffer.data()), buffer.size()));
    }
}
