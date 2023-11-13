#pragma once

#include <list>
#include <map>

#include <rpp/subjects/publish_subject.hpp>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

class ThingsRepository;

using rpp::dynamic_observable;
using rpp::subjects::publish_subject;

#define _site Site::instance()

class Site {
public:
    struct SiteData {
        int ts = 0;
        int pvPower = 0;
        int gridPower = 0;
        int sitePower = 0;
    };

    static Site* instance();

    void setProperty(MutableProperty property, const Value& value) const;
    const std::map<MutableProperty, Value>& mutableProperties() const;

    dynamic_observable<std::map<Property, Value>> properties() const;

    const std::list<SiteData>& history() const;

private:
    Site();

    static inline Site* _instance;

    publish_subject<std::pair<std::string, int>> _pvPowers;
    publish_subject<int> _pvPower;
    publish_subject<int> _gridPower;
    publish_subject<SiteData> _siteDataSubject;

    mutable std::map<MutableProperty, Value> _mutableProperties;
    publish_subject<std::map<Property, Value>> _propertiesSubject;

    std::list<SiteData> _history;
};
