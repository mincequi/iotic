#include "GoeCharger.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <common/Logger.h>

AbstractThingPtr GoeCharger::from(const ThingInfo& thingInfo) {
    if (std::regex_match(thingInfo.id(), std::regex("go-eCharger_\\d{6}"))) {
        return AbstractThingPtr(new GoeCharger(thingInfo));
    }
    return nullptr;
}

void GoeCharger::read() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(id() + "/api/status?filter=nrg");
}

void GoeCharger::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    const auto val = doc["nrg"].toArray();
    _voltage[0] = val.at(0).toDouble();
    _voltage[1] = val.at(1).toDouble();
    _voltage[2] = val.at(2).toDouble();

    _amperage[0] = val.at(4).toDouble();
    _amperage[1] = val.at(5).toDouble();
    _amperage[2] = val.at(6).toDouble();

    _power = val.at(11).toDouble();

    LOG_S(INFO) << "response> " << response.toStdString();
}
