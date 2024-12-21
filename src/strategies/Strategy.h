#pragma once

#include <string>

#include <uvw_iot/util/Site.h>

using uvw_iot::util::Site;

class Strategy {
public:
    virtual ~Strategy();

    const std::string& thingId() const;

    //virtual const std::string& name() const = 0;
    virtual void evaluate(const Site::Properties& siteProperties) = 0;

protected:
    Strategy(const std::string& thingId);

private:
    const std::string _thingId;
};
