#include "WebServer.h"

#include <App.h>
#include <cmrc/cmrc.hpp>
#include <nlohmann/json.hpp>
#include <uvw/loop.h>
#include <uvw_iot/ThingRepository.h>

#include <common/Logger.h>
#include <common/Util.h>
#include <config/ConfigRepository.h>
#include <rules/RulesEngine.h>
#include <rules/SymbolRepository.h>
#include <strategies/Strategy.h>
#include <strategies/StrategyRepository.h>
#include <things/ThingValue.h>

#include "OcppBehavior.h"
#include "WebAppBehavior.h"

CMRC_DECLARE(webapp);

using json = nlohmann::json;
using namespace uvw_iot;
using namespace uvw_iot::util;

WebServer::WebServer(const ThingRepository& repo,
                     const Site& site,
                     const ConfigRepository& cfg,
                     const StrategyRepository& strategyRepository,
                     const SymbolRepository& symbolRepository) :
    _repo(repo),
    _site(site),
    _cfg(cfg),
    _strategyRepository(strategyRepository),
    _symbolRepository(symbolRepository) {
    _fs = std::make_unique<cmrc::embedded_filesystem>(cmrc::webapp::get_filesystem());
    uWS::Loop::get(uvw::loop::get_default()->raw());
    _uwsApp = std::make_unique<uWS::App>();
    _webAppRouter = std::make_shared<WebAppRouter>(repo, site, cfg);
    _uwsApp->get("/", [this](uWS::HttpResponse<false>* res, auto* req) {
        auto f = _fs->open("index.html");
        res->end(std::string_view(f.begin(), f.end()-f.begin()));
    });
    _uwsApp->get("/*", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        const auto fileName = std::string(req->getUrl());
        LOG_S(INFO) << "get> " << fileName;

        if (!_fs->exists(fileName)) {
            LOG_S(ERROR) << "file not found: " << fileName;
            res->end("file not found: " + fileName);
            return;
        }
        auto f = _fs->open(fileName);
        res->end(std::string_view(f.begin(), f.end()-f.begin()));
    });
    _uwsApp->get("/symbols", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        json symbols;
        for (const auto& kv : _symbolRepository.symbolTable()) {
            symbols[kv.first] = kv.second;
        }
        res->end(symbols.dump(0));
    });
    _uwsApp->get("/strategies", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        json strategies = json::array();
        for (const auto& s : _strategyRepository.strategies()) {
            strategies.push_back(s->toJson());
        }
        res->end(strategies.dump(2));
    });
    _uwsApp->ws<WebAppRouterPtr>("/ws", WebAppBehavior::create(_webAppRouter));
    //_uwsApp->ws<UserData>("/ocpp", OcppBehavior::create());
    _uwsApp->ws<UserData>("/ocpp/*", OcppBehavior::create());
    _uwsApp->listen(8030, [](auto* listenSocket) {
        if (!listenSocket) {
            LOG_S(FATAL) << "port 8030 already in use";
        }
    });

    _site.properties().subscribe([this](const Site::Properties& props) {
        json siteProperties;
        siteProperties[::util::toString(ThingPropertyKey::timestamp)] = props.ts;
        siteProperties[::util::toString(ThingPropertyKey::grid_power)] = props.gridPower;
        siteProperties[::util::toString(ThingPropertyKey::pv_power)] = props.pvPower;
        siteProperties[::util::toString(ThingPropertyKey::short_term_grid_power)] = props.shortTermGridPower;
        siteProperties[::util::toString(ThingPropertyKey::long_term_grid_power)] = props.longTermGridPower;

        json json;
        json["site"] = siteProperties;
        _uwsApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT);
    });

    _cfg.thingIntervalObservable().subscribe([this](int value) {
        json properties;
        properties[::util::toString(ThingPropertyKey::thing_interval)] = value;

        json json;
        json["site"] = properties;
        _uwsApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT);
    });

    _repo.thingAdded().subscribe([this](ThingPtr thing) {
        // When new thing is added, send persistent properties to each client
        _uwsApp->publish("broadcast",
                         WebAppBehavior::serializeUserProperties(thing),
                         uWS::OpCode::TEXT);

        thing->propertiesObservable().subscribe([this, thing](const ThingPropertyMap& prop) {
            json thing_;
            for (const auto& kv : prop) {
                if (kv.first <= ThingPropertyKey::next_phases)
                    thing_[::util::toString(kv.first)] = toJsonValue(kv.second);
            }
            if (thing_.empty()) return;

            json json;
            json[thing->id()] = thing_;
            _uwsApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT);
        });
    });

    // _strategyRepo.strategiesObservable().subscribe([this](const std::list<std::shared_ptr<Strategy>>& strategies) {
    //     json json;
    //     for (const auto& strategy : strategies) {
    //         json["strategies"][strategy->thingId()] = strategy->toJson();
    //     }
    //     _uwsApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT);
    // });
}

WebServer::~WebServer() {
}

void WebServer::registerGetRoute(const std::string& path, uWS::MoveOnlyFunction<void(uWS::HttpResponse<false>*, uWS::HttpRequest*)>&& handler) const {
    _uwsApp->get(path, std::move(handler));
}
