#pragma once

#include <map>
#include <memory>
#include <common/Logger.h>
#include <rpp/observables/connectable_observable.hpp>
#include <rpp/observables/dynamic_observable.hpp>
#include <rpp/subjects/publish_subject.hpp>
#include <rpp/subjects/behavior_subject.hpp>
#include <things/ThingInfo.h>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

using rpp::dynamic_observable;
using rpp::subjects::publish_subject;

class Thing : public ThingInfo {
public:
    enum class State {
        Connected,
        Failed
    };

    enum class Type {
        Undefined = 0,
        SmartMeter,
        SolarInverter,
        EvStation,
        Relay
    };

    explicit Thing(const ThingInfo& info);
    virtual ~Thing();

    Type type() const;
    // Custom properties which are loaded from config
    std::string name() const;
    uint16_t icon() const;
    bool isOnSite() const;

    // If thing does not fire updates itself, this can be called to trigger it.
    void read();

    // State shall be communicated via on_error, however we keep this for a rx implementation without exceptions.
    dynamic_observable<State> state();

    virtual void setProperty(WriteableThingProperty property, ThingValue value);
    dynamic_observable<std::map<ReadableThingProperty, ThingValue>> properties() const;

protected:
    virtual void doRead() = 0;

    Type _type = Type::Undefined;
    std::string _name;
    uint16_t _materialIcon = 0;
    bool _isOnSite = false;

    // Inherited classes shall use these to fire updates.
    publish_subject<State> _state;
    publish_subject<std::map<ReadableThingProperty, ThingValue>> _propertiesSubject;
    dynamic_observable<std::map<ReadableThingProperty, ThingValue>> _propertiesObservable;
};

using ThingPtr = std::unique_ptr<Thing>;
