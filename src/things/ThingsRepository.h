#pragma once

#include <list>
#include <things/Site.h>
#include <things/Thing.h>

using rpp::subjects::publish_subject;

#define repo ThingsRepository::instance()

/**
 * @brief The ThingsRepository class
 */
class ThingsRepository {
public:
    static ThingsRepository* instance();

    const Site& site() const;

    void addThing(ThingPtr&& thing);
    // According to: https://stackoverflow.com/questions/43400850/return-a-unique-ptr-by-reference
    // https://www.chromium.org/developers/smart-pointer-guidelines/
    // we return rawpointer here.
    const ThingPtr& thingById(const std::string& id) const;
    const Thing* thingByHost(const std::string& host) const;

    const std::list<ThingPtr>& things() const;

    // TODO: there are issues with that. If we use raw pointer instead of unique_ptr, subscribers always
    // seem to obtain same id for different things...
    // Also access into this thing causes crashes. For now changed vector<ThingsPtr> to list<ThingsPtr>.
    // Let's see, if that helps.
    dynamic_observable<ThingPtr> thingAdded() const;
    dynamic_observable<std::string> thingRemoved() const;

    void setThingProperty(const std::string& id, MutableProperty property, Value value) const;

private:
    ThingsRepository();

    static inline ThingsRepository* _instance;

    std::list<ThingPtr> _things;
    publish_subject<ThingPtr> _thingAdded;
    publish_subject<std::string> _thingRemoved;
    Site _site;
};
