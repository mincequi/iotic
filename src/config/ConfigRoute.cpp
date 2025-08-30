#include "ConfigRoute.h"

#include <HttpResponse.h>
#include <webserver/WebServer.h>

ConfigRoute::ConfigRoute(const WebServer& webServer) {
    webServer.registerGetRoute("/config", [](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
        res->end("config");
    });
}
