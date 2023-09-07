#pragma once

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
        int pvPower = 0;
        int gridPower = 0;
        int sitePower = 0;
    };

    // TODO: make site data a map of ReadableThingProperties and values
    dynamic_observable<SiteData> siteData() const;
    dynamic_observable<std::map<ReadableThingProperty, ThingValue>> properties() const;
    //dynamic_observable<std::map<ReadableThingProperty, double>> properties() const;

private:
    publish_subject<std::pair<std::string, int>> _pvPowers;
    publish_subject<int> _pvPower;
    publish_subject<int> _gridPower;
    publish_subject<SiteData> _siteData;
    publish_subject<std::map<ReadableThingProperty, ThingValue>> _properties;
};
