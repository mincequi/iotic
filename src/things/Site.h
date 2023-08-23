#pragma once

#include <rpp/subjects/publish_subject.hpp>

class ThingsRepository;

class Site {
public:
    Site(const ThingsRepository& thingsRepository);

    struct SiteData {
        int pvPower = 0;
        int gridPower = 0;
        int sitePower = 0;
    };

    rpp::dynamic_observable<SiteData> siteData() const;

private:
    rpp::subjects::publish_subject<std::pair<std::string, int>> _pvPowers;
    rpp::subjects::publish_subject<int> _pvPower;
    rpp::subjects::publish_subject<int> _gridPower;
    rpp::subjects::publish_subject<SiteData> _siteData;
};
