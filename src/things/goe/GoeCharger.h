#pragma once

#include <array>
#include <things/http/HttpThing.h>

class GoeCharger : HttpThing {
public:
    static ThingPtr from(const ThingInfo& info);

private:
    GoeCharger(const ThingInfo& info);
    virtual void doSetProperty(MutableProperty property, Value value) override;
    void doRead() override;
    void onRead(const QByteArray& response) override;

    // site values
    double _availablePower = 0;

    // ev station values
    std::array<double, 3> _voltage;
    double _power = 0.0;
    int _phases = 0;
    int _amperage = 0;
};
