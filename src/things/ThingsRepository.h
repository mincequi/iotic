#pragma once

#include <list>
#include <things/Site.h>
#include <things/Thing.h>

class ThingsDiscovery;

using rpp::subjects::publish_subject;

class ThingsRepository {
public:
    ThingsRepository();

    const Site& site() const;

    void addThing(ThingPtr&& thing);
    // According to: https://stackoverflow.com/questions/43400850/return-a-unique-ptr-by-reference
    // https://www.chromium.org/developers/smart-pointer-guidelines/
    // we return rawpointer here.
    Thing* thingById(const std::string& id) const;
    const Thing* thingByHost(const std::string& host) const;

    dynamic_observable<std::list<ThingPtr>> things() const;

    // TODO: there are issues with that. If we use raw pointer instead of unique_ptr, subscribers always
    // seem to obtain same id for different things...
    // Also access into this thing causes crashes. For now changed vector<ThingsPtr> to list<ThingsPtr>.
    // Let's see, if that helps.
    dynamic_observable<ThingPtr> thingAdded() const;

private:
    std::list<ThingPtr> _things;
    publish_subject<std::list<ThingPtr>> _thingsSubject;
    publish_subject<ThingPtr> _thingAdded;
    Site _site;
};
