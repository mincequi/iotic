#pragma once

#include <memory>
#include <rpp/observables/dynamic_observable.hpp>
#include <rpp/subjects/publish_subject.hpp>
#include <things/ThingInfo.h>

using rpp::dynamic_observable;
using rpp::subjects::publish_subject;

class ThingsDiscovery {
public:
    ThingsDiscovery();
    virtual ~ThingsDiscovery();

    virtual void start(uint16_t seconds = 60) = 0;
    virtual void stop() = 0;

    virtual const dynamic_observable<ThingInfo> thingDiscovered() const;

protected:
    publish_subject<ThingInfo> _thingDiscovered;
};

using ThingsDiscoveryPtr = std::unique_ptr<ThingsDiscovery>;
