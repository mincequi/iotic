#pragma once

#include <string>
#include <common/Types.h>

#include <uvw_iot/util/Site.h>

using uvw_iot::util::Site;

class Strategy {
public:
    enum class Step {
        Keep,
        Down,
        Up
    };

    virtual ~Strategy();

    inline const std::string& thingId() const { return _thingId; }
    inline int priority() const { return _priority; }

    virtual json toJson() const;

    // We separate wantsToStepDown and wantsToSwitchOn to allow priority-based
    // decision making in the PowerManager.
    virtual bool wantsToStepDown(const Site::Properties& siteProperties) const = 0;
    virtual bool wantsToStepUp(const Site::Properties& siteProperties) const = 0;
    virtual void adjust(Step step, const Site::Properties& siteProperties) = 0;
    virtual int measuredPower() const = 0;

protected:
    Strategy(const std::string& thingId);

private:
    friend class StrategyFactory;

    const std::string _thingId;
    int _priority = 0;
};
