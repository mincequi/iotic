#pragma once

#include <map>
#include <string>

#include <things/ThingProperty.h>
#include <things/ThingValue.h>

class Strategy {
public:
    virtual ~Strategy();

    const std::string& thingId() const;

    //virtual const std::string& name() const = 0;
    virtual void evaluate(double gridPower) = 0;

protected:
    Strategy(const std::string& thingId);

private:
    const std::string _thingId;
};
