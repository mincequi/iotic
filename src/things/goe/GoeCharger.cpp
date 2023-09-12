#include "GoeCharger.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <common/Logger.h>

ThingPtr GoeCharger::from(const ThingInfo& info) {
    if (std::regex_match(info.host(), std::regex("go-eCharger_\\d{6}"))) {
        return ThingPtr(new GoeCharger(info));
    }
    return nullptr;
}

GoeCharger::GoeCharger(const ThingInfo& info) :
    HttpThing(info) {
    _type = Thing::Type::EvStation;
    _isOnSite = true;
}

void GoeCharger::doSetProperty(WriteableThingProperty property, ThingValue value) {
    if (property != WriteableThingProperty::power_control) return;

    _availablePower = std::get<double>(value);
}

void GoeCharger::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(host() + "/api/status?filter=amp,nrg,psm,"); // psm (for phases) and amp (for amps)
}

void GoeCharger::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    _amperage = doc["amp"].toInt();
    const auto nrg = doc["nrg"].toArray();
    _voltage.at(0) = nrg.at(0).toInt();
    _voltage.at(1) = nrg.at(1).toInt();
    _voltage.at(2) = nrg.at(2).toInt();
    _power = nrg.at(11).toDouble();
    static int phases[] = { 0, 1, 3};
    _phases = phases[doc["psm"].toInt()];
    _propertiesSubject.get_subscriber().on_next({{ReadableThingProperty::power, _power}});
}
