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
    struct SiteData {
        int ts = 0;
        int pvPower = 0;
        int gridPower = 0;
        int sitePower = 0;
    };

    Site(const ThingsRepository& thingsRepository);

    dynamic_observable<std::map<Property, Value>> properties() const;

    const std::list<SiteData>& history() const;

private:
    publish_subject<std::pair<std::string, int>> _pvPowers;
    publish_subject<int> _pvPower;
    publish_subject<int> _gridPower;
    publish_subject<SiteData> _siteDataSubject;
    publish_subject<std::map<Property, Value>> _propertiesSubject;

    std::list<SiteData> _history;
};
