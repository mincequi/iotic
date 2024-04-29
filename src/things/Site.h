#pragma once

#include <list>
#include <map>

#include <rpp/subjects/publish_subject.hpp>
#include <common/Logger.h>

namespace uvw_iot {
namespace common {
class ThingRepository;
} // namespace common
} // namespace uvw_iot

using rpp::dynamic_observable;
using rpp::subjects::publish_subject;
using uvw_iot::common::ThingRepository;

class Config;

class Site {
public:
    struct SiteData {
        int ts = 0;
        int pvPower = 0;
        int gridPower = 0;
    };

    Site(const ThingRepository& repo, const Config& cfg);

    void setProperty(uvw_iot::common::ThingPropertyKey property, const uvw_iot::common::ThingPropertyValue& value) const;
    const uvw_iot::common::ThingPropertyMap& mutableProperties() const;

    dynamic_observable<uvw_iot::common::ThingPropertyMap> properties() const;
    dynamic_observable<int> gridPower() const;

    const std::list<SiteData>& history() const;

private:
    const ThingRepository& _repo;
    const Config& _cfg;
    publish_subject<std::pair<std::string, int>> _pvPowers;
    publish_subject<int> _pvPower;
    publish_subject<int> _gridPower;
    publish_subject<SiteData> _siteDataSubject;

    mutable uvw_iot::common::ThingPropertyMap _mutableProperties;
    publish_subject<uvw_iot::common::ThingPropertyMap> _propertiesSubject;

    std::list<SiteData> _history;
};
