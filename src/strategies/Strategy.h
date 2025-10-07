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
    inline State state() const { return _state; }

    //virtual const std::string& name() const = 0;
    virtual void evaluate(const Site::Properties& siteProperties) const = 0;

    virtual json toJson() const;

protected:
    Strategy(const std::string& thingId);

private:
    friend class StrategyFactory;

    const std::string _thingId;
    double _priority = 0.0;
    State _state = State::Off;
    int _lastActuationTs = 0;
};
