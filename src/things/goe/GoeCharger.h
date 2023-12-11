#pragma once

#include <array>
#include <things/ThingStatus.h>
#include <things/http/HttpThing.h>

class GoeCharger : public HttpThing {
public:
    static ThingPtr from(const ThingInfo& info);
    ~GoeCharger();

private:
    GoeCharger(const ThingInfo& info);

    virtual void doSetProperty(MutableProperty property, const Value& value) override;
    void doRead() override;
    void onRead(const std::string& response) override;

    ThingStatus _status = ThingStatus::unknown;
};
