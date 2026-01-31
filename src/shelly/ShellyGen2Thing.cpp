#include "ShellyGen2Thing.h"

#include <rfl/Generic.hpp>
#include <rfl/Object.hpp>
#include <rfl/json.hpp>

using namespace uvw_iot;

ThingPtr ShellyGen2Thing::from(const uvw_net::dns_sd::MdnsResponse& mdnsResponse) {
    if (!mdnsResponse.srvData) {
        return nullptr;
    }

    const auto& srv = mdnsResponse.srvData.value();
    // Get hsotname by truncating at the first dot
    const std::string host = srv.target.substr(0, srv.target.find('.'));
    const uint16_t port = srv.port;

    return ThingPtr(new ShellyGen2Thing(host, port));
}

ShellyGen2Thing::ShellyGen2Thing(const std::string& host, uint16_t port)
    : HttpThing(host, port) {
}

void ShellyGen2Thing::fetchProperties() {
    get("/rpc/Shelly.GetStatus");
}

void ShellyGen2Thing::onSetProperties(const ThingPropertyMap& properties) {
    properties.on<ThingPropertyKey::power_control>([&](const auto& isOn) {
        const std::string strValue = isOn ? "on" : "off";
        set("/relay/0?turn=" + strValue);
    });
}

void ShellyGen2Thing::onBody(const std::string& body) {
    std::vector<bool> inputs;
    std::vector<bool> switches;
    int phases = 0;

    rfl::json::read<rfl::Generic::Object>(body).and_then([&](const rfl::Generic::Object& obj) {
        for (const auto& [key, value] : obj) {
            // -------- inputs --------
            if (key.starts_with("input:")) {
                value.to_object().and_then([&](const rfl::Generic::Object& inputObj) {
                    inputObj.get("state")->to_bool().and_then([&](bool state) {
                        inputs.push_back(state);
                    });
                });
            }

            // -------- switches --------
            else if (key.starts_with("switch:")) {
                value.to_object().and_then([&](const rfl::Generic::Object& switchObj) {
                    switchObj.get("output")->to_bool().and_then([&](bool output) {
                        switches.push_back(output);
                        ++phases;
                    });
                });
            }
        }
    });

    ThingPropertyMap properties;
    properties.set<ThingPropertyKey::multistateSelector>(switches);
    properties.set<ThingPropertyKey::digitalInput>(inputs);
    publish(properties);
}
