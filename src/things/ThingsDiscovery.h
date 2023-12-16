#pragma once

#include <memory>
#include <common/Rpp.h>
#include <things/Thing.h>

class ThingsDiscovery {
public:
    ThingsDiscovery();
    virtual ~ThingsDiscovery();

    virtual void start(int msec);
    virtual void stop();

    const dynamic_observable<ThingPtr> thingDiscovered() const;

protected:
    dynamic_subscriber<ThingPtr> thingDiscoveredSubscriber() const;

private:
    publish_subject<ThingPtr> _thingDiscovered;
};

using ThingsDiscoveryPtr = std::shared_ptr<ThingsDiscovery>;
