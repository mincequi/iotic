#pragma once

#include <string>

class Rule {
public:
    Rule(const std::string& thingId);
    virtual ~Rule();

    const std::string& thingId() const;

    //virtual const std::string& name() const = 0;
    virtual void evaluate() = 0;

private:
    const std::string _thingId;
};
