#pragma once

#include <string>

class Strategy {
public:
    Strategy(const std::string& thingId);
    virtual ~Strategy();

    const std::string& thingId() const;

    //virtual const std::string& name() const = 0;
    virtual void evaluate() = 0;

private:
    const std::string _thingId;
};
