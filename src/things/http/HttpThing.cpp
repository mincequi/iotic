#include "HttpThing.h"

#include <common/Logger.h>
#include <things/ThingsRepository.h>

#include "client.h"
#include "common.h"

using namespace std::placeholders;

struct HttpThing::Impl {
    http::client client;
};

HttpThing::HttpThing(const ThingInfo& info)
    : Thing{info},
      _p(std::make_unique<Impl>()) {
}

HttpThing::~HttpThing() {
}

void HttpThing::read(const std::string& url) {
    http::request req;
    req.url = "http://" + url;
    _p->client.fetch(req, std::bind(&ThingsRepository::onRead, repo, id(), _1, _2));
}

void HttpThing::write(const std::string& url) {
    http::request req;
    req.url = "http://" + url;
    _p->client.fetch(req, std::bind(&ThingsRepository::onReadOnlyError, repo, id(), _2));
}
