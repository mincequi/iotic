#pragma once

#include <list>
#include <map>

#include <rpp/subjects/publish_subject.hpp>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

class ThingsRepository;

using rpp::dynamic_observable;
using rpp::subjects::publish_subject;

class Site {
public:
    Site(const ThingsRepository& thingsRepository);

    struct SiteData {
        int ts = 0;
        int pvPower = 0;
        int gridPower = 0;
        int sitePower = 0;
    };

    dynamic_observable<std::map<Property, Value>> properties() const;

    const std::list<SiteData>& history() const;

private:
    publish_subject<std::pair<std::string, int>> _pvPowers;
    publish_subject<int> _pvPower;
    publish_subject<int> _gridPower;
    publish_subject<SiteData> _siteData;
    publish_subject<std::map<Property, Value>> _properties;

    std::list<SiteData> _history;
};
