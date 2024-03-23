#include "GoeCharger.h"

#include <regex>
#include <QJsonArray>
#include <QJsonDocument>
#include <common/Logger.h>

#include "GoeUtil.h"

ThingPtr GoeCharger::from(const ThingInfo& info) {
    if (std::regex_match(info.host(), std::regex("go-eCharger_\\d{6}"))) {
        return ThingPtr(new GoeCharger(info));
    }
    return nullptr;
}

GoeCharger::GoeCharger(const ThingInfo& info) :
    HttpThing(info) {
    _type = Thing::Type::EvStation;
}

GoeCharger::~GoeCharger() {
}

void GoeCharger::doSetProperty(MutableProperty property, const Value& value) {
    if (_status != ThingStatus::waiting && _status != ThingStatus::charging) return;

    switch (property) {
    case MutableProperty::current:
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double>) {
                if (arg == 0.0) {
                    read(host(), "/api/set?frc=1");
                } else {
                    read(host(), "/api/set?psm=1"); // force 1 phase
                    read(host(), "/api/set?amp=" + std::to_string(std::clamp((int)std::round(arg), 6, 32)));
                    read(host(), "/api/set?frc=2"); // switch on
                }
            } else if constexpr (std::is_same_v<T, std::array<double, 3>>) {
                read(host(), "/api/set?psm=2"); // force 3 phase
                read(host(), "/api/set?amp=" + std::to_string(std::clamp((int)std::round(arg.front()), 6, 32)));
                read(host(), "/api/set?frc=2");
            }
        }, value);
        break;
    default:
        break;
    }
}

void GoeCharger::doRead() {
    // alw,car,eto,nrg,wh,trx,cards"
    HttpThing::read(host(), "/api/status?filter=nrg,car"); // psm (for phases) and amp (for amps)
}

void GoeCharger::onRead(const std::string& response) {
    const auto doc = QJsonDocument::fromJson(QByteArray::fromStdString(response));
    const auto nrg = doc["nrg"].toArray();
    if (nrg.isEmpty()) return;

    std::array<double, 3> voltage;
    voltage.at(0) = nrg.at(0).toDouble();
    voltage.at(1) = nrg.at(1).toDouble();
    voltage.at(2) = nrg.at(2).toDouble();

    _status = goe::toStatus(doc["car"].toInt());

    propertiesSubscriber().on_next({
        { Property::status, static_cast<int>(_status) },
        { Property::power, nrg.at(11).toDouble() },
        { Property::voltage, voltage }
    });
}
