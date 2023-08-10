#pragma once

#include <Thing.h>

using rxcpp::observable;
using rxcpp::subjects::behavior;
using rxcpp::subjects::subject;

class ThingsDiscovery;

class ThingsRepository {
public:
    ThingsRepository(const std::vector<ThingsDiscovery>& discoveries);

    void addThing(ThingPtr thing);

    observable<std::vector<ThingPtr>> things() const;

private:
    const std::vector<ThingsDiscovery>& _discoveries;
    std::vector<ThingPtr> _things;
    subject<std::vector<ThingPtr>> _thingsSubject;
};
