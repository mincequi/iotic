#pragma once

#include <memory>
#include <rpp/observables/dynamic_observable.hpp>
#include <rxcpp/rx-lite.hpp>
#include <things/ThingProperty.h>

using rpp::dynamic_observable;
using rxcpp::grouped_observable;

class Thing {
public:
    Thing(/*const std::string& id*/);

    const std::string& id() const;

    void setProperty(WriteableThingProperty property, double value);
    grouped_observable<ReadableThingProperty, double> properties() const;

protected:
    virtual void doSetProperty(WriteableThingProperty property, double value);
    virtual grouped_observable<ReadableThingProperty, double> doProperties() const;

private:
    std::string _id;
};

using ThingPtr = std::unique_ptr<Thing>;
