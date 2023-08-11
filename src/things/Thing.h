#pragma once

#include <memory>
#include <common/Logger.h>
#include <rpp/observables/dynamic_observable.hpp>
#include <rpp/subjects/publish_subject.hpp>
#include <things/ThingInfo.h>
#include <things/ThingProperty.h>

using rpp::dynamic_observable;
//using rpp::grouped_observable;
using rpp::subjects::publish_subject;
//using rxcpp::dynamic_grouped_observable;
//using rxcpp::subjects::subject;

class Thing : public ThingInfo {
public:
    enum class State {
        Connected,
        Failed
    };

    explicit Thing(const ThingInfo& info);
    virtual ~Thing();

    // If thing does not fire updates itself, this can be called to trigger it.
    void read();

    // State shall be communicated via on_error, however we keep this for a rx implementation without exceptions.
    dynamic_observable<State> state();

    virtual void setProperty(WriteableThingProperty property, double value);
    dynamic_observable<std::pair<ReadableThingProperty, double>> properties();

protected:
    virtual void doRead() = 0;

    // Inherited classes shall use these to fire updates.
    publish_subject<State> _state;
    publish_subject<std::pair<ReadableThingProperty, double>> _properties;
};

using ThingPtr = std::unique_ptr<Thing>;
