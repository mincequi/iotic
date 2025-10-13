#pragma once

#include <string>
#include <common/Types.h>

#include <uvw_iot/util/Site.h>

using uvw_iot::util::Site;

class Strategy {
public:
    enum class State {
        Off,
        SwitchingOn,
        On,
        SwitchingOff,
    };

    virtual ~Strategy();

    inline const std::string& thingId() const { return _thingId; }
    inline double priority() const { return _priority; }

    //virtual const std::string& name() const = 0;
    virtual void evaluate(const Site::Properties& siteProperties) const = 0;
    virtual bool wantsToTurnOff(const Site::Properties& siteProperties) = 0;
    virtual bool wantsToTurnOn(const Site::Properties& siteProperties) = 0;

    virtual json toJson() const;

protected:
    Strategy(const std::string& thingId);

private:
    friend class StrategyFactory;

    const std::string _thingId;
    double _priority = 0.0;
};
