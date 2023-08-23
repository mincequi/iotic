#include "GoeCharger.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <common/Logger.h>

ThingPtr GoeCharger::from(const ThingInfo& info) {
    if (std::regex_match(info.id(), std::regex("go-eCharger_\\d{6}"))) {
        return ThingPtr(new GoeCharger(info));
    }
    return nullptr;
}

void GoeCharger::setProperty(WriteableThingProperty property, double value) {

}

void GoeCharger::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(id() + "/api/status?filter=nrg");
}

void GoeCharger::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    const auto val = doc["nrg"].toArray();
    _propertiesSubject.get_subscriber().on_next({{ReadableThingProperty::power, val.at(11).toDouble()}});
}
