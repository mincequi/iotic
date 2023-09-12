#include "Shelly.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <common/Logger.h>

#include <iostream>

ThingPtr Shelly::from(const ThingInfo& info) {
    std::string str = info.host();
    std::regex rgx("shelly[1-4](pm)*-[0-9A-F]{12}");
    std::smatch matches;
    auto match = std::regex_match(str, matches, rgx);
    if (match) {
        return ThingPtr(new Shelly(info, !matches[1].str().empty()));
    }
    return nullptr;
}

Shelly::Shelly(const ThingInfo& info, bool isPm) :
    HttpThing(info),
    _isPm(isPm) {
    _type = Thing::Type::Relay;
}

void Shelly::doSetProperty(WriteableThingProperty property, ThingValue value) {
    switch (property) {
    case WriteableThingProperty::power_control: {
        const std::string strValue = std::get<bool>(value) ? "on" : "off";
        write(host() + "/relay/0?turn=" + strValue);
        break;
    }
    default:
        break;
    }
}

void Shelly::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(host() + "/status");
    //setProperty(WriteableThingProperty::powerControl, 1.0);
}

void Shelly::onRead(const QByteArray& response) {
    const auto doc = QJsonDocument::fromJson(response);
    std::map<ReadableThingProperty, ThingValue> properties;
    {
        const auto val = doc["relays"][0]["ison"];
        if (!val.isUndefined())
            properties[ReadableThingProperty::power_control] = val.toBool();
    } if (_isPm) {
        const auto val = doc["meters"][0]["power"];
        if (!val.isUndefined())
            properties[ReadableThingProperty::power] = val.toDouble();
    } {
        const auto val = doc["ext_temperature"]["0"]["tC"];
        if (!val.isUndefined())
            properties[ReadableThingProperty::temperature] = val.toDouble();
    }

    if (!properties.empty()) {
        _propertiesSubject.get_subscriber().on_next(properties);
    }
}
