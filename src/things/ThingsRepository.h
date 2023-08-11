#pragma once

#include <Thing.h>

class ThingsDiscovery;

class ThingsRepository {
public:
    ThingsRepository(const std::vector<ThingsDiscovery>& discoveries);

    void addThing(ThingPtr thing);

    dynamic_observable<std::vector<ThingPtr>> things() const;

private:
    const std::vector<ThingsDiscovery>& _discoveries;
    std::vector<ThingPtr> _things;
    publish_subject<std::vector<ThingPtr>> _thingsSubject;
};
