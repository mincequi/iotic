#pragma once

#include <uvw_iot/http/HttpThing.h>
#include <uvw_net/dns_sd/MdnsResponse.h>

namespace uvw_net {
namespace dns_sd {
struct MdnsResponse;
} // namespace dns_sd
} // namespace uvw_net

class ShellyGen2Thing : public uvw_iot::http::HttpThing {
public:
    static uvw_iot::ThingPtr from(const uvw_net::dns_sd::MdnsResponse& mdnsResponse);

private:
    ShellyGen2Thing(const std::string& host, uint16_t port);

    // Thing overrides
    inline uvw_iot::ThingType type() const override { return uvw_iot::ThingType::Relay; }
    void fetchProperties() override;
    void onSetProperties(const uvw_iot::ThingPropertyMap& properties) override;

    // HttpThing overrides
    void onBody(const std::string& body) override;

    int _phaseToSet = 0;
};
