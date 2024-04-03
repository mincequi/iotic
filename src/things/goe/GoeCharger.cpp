#include "GoeCharger.h"

#include <cmath>
#include <regex>
#include <nlohmann/json.hpp>

#include <common/Logger.h>

#include "GoeUtil.h"

using json = nlohmann::json;

ThingPtr GoeCharger::from(const ThingInfo& info) {
    if (std::regex_match(info.host(), std::regex("go-echarger_\\d{6}"))) {
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
                    write(host(), "/api/set?frc=1");
                } else {
                    write(host(), "/api/set?psm=1"); // force 1 phase
                    write(host(), "/api/set?amp=" + std::to_string(std::clamp((int)std::round(arg), 6, 32)));
                    write(host(), "/api/set?frc=2"); // switch on
                }
            } else if constexpr (std::is_same_v<T, std::array<double, 3>>) {
                write(host(), "/api/set?psm=2"); // force 3 phase
                write(host(), "/api/set?amp=" + std::to_string(std::clamp((int)std::round(arg.front()), 6, 32)));
                write(host(), "/api/set?frc=2");
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
    if (response.empty()) {
        LOG_S(WARNING) << id() << "> response is empty";
        return;
    }

    DLOG_S(1) << id() << "> response: " << response;
    const auto doc = json::parse(response);
    if (!doc.contains("nrg")) return;

    const auto nrg = doc["nrg"].get<std::vector<double>>();
    if (nrg.empty()) return;

    std::array<double, 3> voltage;
    voltage.at(0) = nrg[0];
    voltage.at(1) = nrg[1];
    voltage.at(2) = nrg[2];

    _status = goe::toStatus(doc["car"].get<int>());

    propertiesSubscriber().on_next({
        { Property::status, static_cast<int>(_status) },
        { Property::power, nrg[11] },
        { Property::voltage, voltage }
    });
}
