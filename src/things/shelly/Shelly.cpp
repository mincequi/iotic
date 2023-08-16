#include "Shelly.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <common/Logger.h>

#include <iostream>

Shelly::Shelly(const ThingInfo& info, bool isPm) :
    HttpThing(info),
    _isPm(isPm) {
}

ThingPtr Shelly::from(const ThingInfo& thingInfo) {
    std::string str = thingInfo.id();
    std::regex rgx("shelly[1-4](pm)*-[0-9A-F]{12}");
    std::smatch matches;
    auto match = std::regex_match(str, matches, rgx);
    if (match) {
        return ThingPtr(new Shelly(thingInfo, !matches[1].str().empty()));
    }
    return nullptr;
}

void Shelly::setProperty(WriteableThingProperty property, double value) {
    switch (property) {
    case WriteableThingProperty::powerControl: {
        const std::string strValue = value == 0.0 ? "off" : "on";
        write(id() + "/relay/0?turn=" + strValue);
        break;
    }
    default:
        break;
    }
}

void Shelly::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(id() + "/status");
    //setProperty(WriteableThingProperty::powerControl, 1.0);
}

void Shelly::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    {
        const auto val = doc["relays"][0]["ison"];
        if (!val.isUndefined())
            _properties.get_subscriber().on_next(std::make_pair(ReadableThingProperty::powerControl, val.toBool()));
    } if (_isPm) {
        const auto val = doc["meters"][0]["power"];
        if (!val.isUndefined())
            _properties.get_subscriber().on_next(std::make_pair(ReadableThingProperty::power, val.toDouble()));
    } {
        const auto val = doc["ext_temperature"]["0"]["tC"];
        if (!val.isUndefined())
            _properties.get_subscriber().on_next(std::make_pair(ReadableThingProperty::temperature, val.toDouble()));
    }
}
