#pragma once

#include <things/Thing.h>

class TestThing : public Thing {
public:
    TestThing(const ThingInfo& info) :
        Thing(info) {
        _type = Type::SmartMeter;
    };

    virtual void doRead() override {};
    virtual void doSetProperty(MutableProperty property, const Value& value) override {};

    void setProperty(Property property, const Value& value) {
        propertiesSubscriber().on_next( {{property, value }} );
    }

    void tick() {
        static int i = 0;
        if (i%4 == 0)
            propertiesSubscriber().on_next({});
        if (i%4 == 1)
            propertiesSubscriber().on_next({});
        if (i%4 == 2)
            propertiesSubscriber().on_next({});
        if (i%4 == 3)
            propertiesSubscriber().on_completed();
        ++i;
    }
};
