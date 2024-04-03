#include "Shelly.h"

#include <regex>
#include <common/Logger.h>

#include <iostream>

using json = nlohmann::json;

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

void Shelly::doSetProperty(MutableProperty property, const Value& value) {
    switch (property) {
    case MutableProperty::power_control: {
        const std::string strValue = std::get<bool>(value) ? "on" : "off";
        write(host(), "/relay/0?turn=" + strValue);
        break;
    }
    default:
        break;
    }
}

void Shelly::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(host(), "/status");
}

void Shelly::onRead(const std::string& response) {
    const auto doc = json::parse(response);
    std::map<Property, Value> properties;
    if (doc.contains("relays")) {
        properties[Property::power_control] = doc.at("relays").at(0).at("ison").get<bool>();
    } else if (doc.contains("ison")) {
        properties[Property::power_control] = doc.at("ison").get<bool>();
    }

    if (_isPm && doc.contains("meters")) {
        properties[Property::power] = doc["meters"][0]["power"].get<double>();
    }
    if (doc.contains("ext_temperature") && !doc.at("ext_temperature").empty()) {
        properties[Property::temperature] = doc.at("ext_temperature").at("0").at("tC").get<double>();
    }

    if (!properties.empty()) {
        propertiesSubscriber().on_next(properties);
    }
}
