#include "WebServer.h"

#include <App.h>
#include <cmrc/cmrc.hpp>
#include <nlohmann/json.hpp>
#include <rfl/json/write.hpp>
#include <uvw/loop.h>
#include <uvw_iot/ThingRepository.h>

#include <common/Logger.h>
#include <common/Util.h>
#include <config/ConfigRepository.h>
#include <database/Database.h>
#include <rules/RuleEngine.h>
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

WebServer::WebServer(const ThingRepository& thingRepository,
                     const Site& site,
                     const ConfigRepository& cfg,
                     const StrategyRepository& strategyRepository,
                     const SymbolRepository& symbolRepository,
                     const Database& database
                     ) :
    _thingRepository(thingRepository),
    _site(site),
    _configRepository(cfg),
    _strategyRepository(strategyRepository),
    _symbolRepository(symbolRepository),
    _database(database) {
    _fs = std::make_unique<cmrc::embedded_filesystem>(cmrc::webapp::get_filesystem());
    uWS::Loop::get(uvw::loop::get_default()->raw());
    _uwsApp = std::make_unique<uWS::App>();
    _webAppRouter = std::make_shared<WebAppRouter>(thingRepository, site, cfg);
    _uwsApp->get("/", [this](uWS::HttpResponse<false>* res, auto* req) {
        auto f = _fs->open("index.html");
        res->end(std::string_view(f.begin(), f.end()-f.begin()));
    });
    _uwsApp->get("/*", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        const auto fileName = std::string(req->getUrl());
        LOG_S(1) << "get> " << fileName;

        if (!_fs->exists(fileName)) {
            LOG_S(ERROR) << "file not found: " << fileName;
            res->end("file not found: " + fileName);
            return;
        }
        auto f = _fs->open(fileName);
        res->end(std::string_view(f.begin(), f.end()-f.begin()));
    });
    _uwsApp->get("/database/info", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        res->end(_database.info());
    });
    _uwsApp->get("/database/stat", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        res->end(_database.stat());
    });
    _uwsApp->get("/database/:thing/:key/:date", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        auto thingId = req->getParameter(0);
        auto key = magic_enum::enum_cast<ThingPropertyKey>(req->getParameter(1));
        if (!key.has_value()) {
            res->end("invalid key: " + std::string{req->getParameter(1)});
            return;
        }
        auto ymdString = std::string{ req->getParameter(2) };
        auto ymd = ::util::parse_ymd(ymdString);
        if (!ymd.ok()) {
            res->end("invalid date: " + ymdString);
            return;
        }
        if (!_database.hasMap(thingId, key.value())) {
            res->end("no data for thing: " + std::string{thingId} + ", key: " + std::string{req->getParameter(1)});
            return;
        }

        auto data = _database.dailyData(thingId, key.value(), ymd);
        res->end(rfl::json::write(data));
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
    _uwsApp->get("/things", [this](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        json things;
        for (const auto& [id, thing] : _thingRepository.things()) {
            json properties;
            thing->properties().forEach([&](ThingPropertyKey key, const auto& value) {
                //if (key <= ThingPropertyKey::voltage)
                    properties[::util::toString(key)] = value;
            });
            if (properties.empty()) continue;

            things[id] = properties;
        }
        res->end(things.dump(2));
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

    _configRepository.thingIntervalObservable().subscribe([this](int value) {
        json properties;
        properties[::util::toString(ThingPropertyKey::thing_interval)] = value;

        json json;
        json["site"] = properties;
        _uwsApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT);
    });

    _thingRepository.thingAdded().subscribe([this](ThingPtr thing) {
        // When new thing is added, send persistent properties to each client
        _uwsApp->publish("broadcast",
                         WebAppBehavior::serializeUserProperties(thing),
                         uWS::OpCode::TEXT);

        thing->propertiesObservable().subscribe([this, thing](const ThingPropertyMap& prop) {
            json thing_;
            prop.forEach([&thing_](ThingPropertyKey key, const auto& value) {
                //if (key <= ThingPropertyKey::voltage)
                    thing_[::util::toString(key)] = value;
            });
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
