#pragma once

#include <array>
#include <things/http/HttpThing.h>

class GoeCharger : HttpThing {
public:
    static ThingPtr from(const ThingInfo& info);

private:
    virtual void setProperty(WriteableThingProperty property, double value) override;
    void doRead() override;
    void onRead(const QByteArray& response) override;

    std::array<double, 3> _voltage;
    std::array<double, 3> _amperage;
    double _power;

    using HttpThing::HttpThing;
};
