#pragma once

#include <map>
#include <memory>

#include <common/Rpp.h>
#include <things/ThingInfo.h>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

class Thing : public ThingInfo {
public:
    enum class Type {
        Undefined = 0,
        SmartMeter,
        SolarInverter,
        EvStation,
        Relay
    };

    enum class State {
        Uninitialized,
        Ready,
        Failed
    };

    explicit Thing(const ThingInfo& info);
    virtual ~Thing();

    Type type() const;
    // Custom properties which are loaded from config
    uint16_t icon() const;

    // If thing does not fire updates itself, this can be called to trigger it.
    void read();

    dynamic_observable<State> stateObservable() const;

    void setProperty(MutableProperty property, const Value& value);
    const std::map<MutableProperty, Value>& mutableProperties() const;
    dynamic_observable<std::map<Property, Value>> properties() const;

protected:
    State state() const;
    dynamic_subscriber<State> stateSubscriber() const;
    dynamic_subscriber<std::map<Property, Value>> propertiesSubscriber() const;

    virtual void onRead(const std::string& response);

    Type _type = Type::Undefined;

private:
    virtual void doRead() = 0;
    virtual void doSetProperty(MutableProperty property, const Value& value);

    uint16_t _materialIcon = 0;

    // TODO: do not store persistent properties here. Use Config as single source of truth.
    std::map<MutableProperty, Value> _mutableProperties;
    publish_subject<std::map<Property, Value>> _propertiesSubject;
    dynamic_observable<std::map<Property, Value>> _propertiesObservable;

    behavior_subject<State> _stateSubject = State::Uninitialized;

    friend class ThingsRepository;
};

using ThingPtr = std::shared_ptr<Thing>;
