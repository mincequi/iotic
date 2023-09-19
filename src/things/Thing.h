#pragma once

#include <map>
#include <memory>

#include <rpp/observables/dynamic_observable.hpp>
#include <rpp/subjects/publish_subject.hpp>

#include <things/ThingInfo.h>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

using rpp::dynamic_observable;
using rpp::subjects::publish_subject;

class Thing : public ThingInfo {
public:
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
    uint16_t icon() const;

    // If thing does not fire updates itself, this can be called to trigger it.
    void read();

    void setProperty(MutableProperty property, const Value& value);
    const std::map<MutableProperty, Value>& mutableProperties() const;

    /**
     * @brief properties
     * @return
     */
    dynamic_observable<std::map<Property, Value>> properties() const;

protected:
    virtual void doRead() = 0;
    virtual void doSetProperty(MutableProperty property, const Value& value) = 0;

    Type _type = Type::Undefined;
    uint16_t _materialIcon = 0;

    // TODO: do not store persistent properties here. Use Config as single source of truth.
    std::map<MutableProperty, Value> _mutableProperties;
    publish_subject<std::map<Property, Value>> _propertiesSubject;
    dynamic_observable<std::map<Property, Value>> _propertiesObservable;
};

using ThingPtr = std::unique_ptr<Thing>;
