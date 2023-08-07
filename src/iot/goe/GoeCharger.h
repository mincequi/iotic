#pragma once

#include <array>
#include <iot/http/HttpThing.h>

class GoeCharger : HttpThing {
public:
    static AbstractThingPtr from(const ThingInfo& ThingInfo);

private:
    void read() override;
    void onRead(const QByteArray& response) override;

    std::array<double, 3> _voltage;
    std::array<double, 3> _amperage;
    double _power;

    using HttpThing::HttpThing;
};
