#include "OcppBehavior.h"

#include <common/Logger.h>

uWS::App::WebSocketBehavior<UserData> OcppBehavior::create() {
    uWS::App::WebSocketBehavior<UserData> behavior;
    behavior.upgrade = [](auto *res, uWS::HttpRequest* req, auto *context) {
        LOG_S(1) << "client url> " << req->getUrl();
        res->template upgrade<UserData>(
                    {},
                    req->getHeader("sec-websocket-key"),
                    req->getHeader("sec-websocket-protocol"),
                    req->getHeader("sec-websocket-extensions"),
                    context);
    };
    behavior.open = [](uWS::WebSocket<false, true, UserData>* ws) {
        LOG_S(1) << "client connected> " << ws->getRemoteAddressAsText();
    };
    behavior.message = [](auto* ws, std::string_view message, uWS::OpCode) {
        LOG_S(1) << "message from " << ws->getRemoteAddressAsText() << "> " << message;
    };

    return behavior;
}
