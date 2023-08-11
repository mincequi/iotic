#include "Shelly.h"

#include "Shelly.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <common/Logger.h>

#include <iostream>

Shelly::Shelly(const ThingInfo& info, bool shallRead) :
    HttpThing(info),
    _shallRead(shallRead) {
}

ThingPtr Shelly::from(const ThingInfo& thingInfo) {
    // shelly1pm-F4CFA2E3849F

    std::string str = thingInfo.id();
    std::regex rgx("shelly[1-4](pm)*-[0-9A-F]{12}");
    std::smatch matches;
    auto match = std::regex_match(str, matches, rgx);
    if (match) {
        return ThingPtr(new Shelly(thingInfo, !matches[1].str().empty()));
    }
    return nullptr;
}

void Shelly::doRead() {
    if (!_shallRead) return;
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(id() + "/status");
}

void Shelly::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    const auto val = doc["meters"].toArray().at(0).toObject()["power"];
    if (val.isUndefined()) return;

    _properties.get_subscriber().on_next(std::make_pair(power, val.toDouble()));
}
