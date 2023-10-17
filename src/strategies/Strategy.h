#pragma once

#include <string>

class Strategy {
public:
    virtual ~Strategy();

    const std::string& thingId() const;

    //virtual const std::string& name() const = 0;
    virtual void evaluate() = 0;

protected:
    Strategy(const std::string& thingId);

private:
    const std::string _thingId;
};
