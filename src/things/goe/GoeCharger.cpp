#include "GoeCharger.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <common/Logger.h>

ThingPtr GoeCharger::from(const ThingInfo& thingInfo) {
    if (std::regex_match(thingInfo.id(), std::regex("go-eCharger_\\d{6}"))) {
        return ThingPtr(new GoeCharger(thingInfo));
    }
    return nullptr;
}

void GoeCharger::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(id() + "/api/status?filter=nrg");
}

void GoeCharger::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    const auto val = doc["nrg"].toArray();
    _properties.get_subscriber().on_next(std::make_pair(power, val.at(11).toDouble()));
}
