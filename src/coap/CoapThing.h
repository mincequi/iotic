#pragma once

#include <uvw_iot/Thing.h>

#include <coap/CoapMessage.h>

using namespace uvw_iot;

class CoapThing : public uvw_iot::Thing {
public:
    static uvw_iot::ThingPtr from(const CoapMessage& message);

private:
    CoapThing(const std::string& id);

    inline const std::string& id() const override { return _id; }
    inline ThingType type() const override { return ThingType::WeatherStation; }

    void fetchProperties() override;

    std::string _id;
};
