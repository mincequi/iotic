#pragma once

#include <array>
#include <things/ThingStatus.h>
#include <things/http/HttpThing.h>

class GoeCharger : HttpThing {
public:
    static ThingPtr from(const ThingInfo& info);

private:
    GoeCharger(const ThingInfo& info);
    ~GoeCharger();
    virtual void doSetProperty(MutableProperty property, const Value& value) override;
    void doRead() override;
    void onRead(const std::string& response) override;

    ThingStatus _status = ThingStatus::unknown;
};
