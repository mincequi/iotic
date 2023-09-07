#pragma once

#include <string>

class Rule {
public:
    Rule(const std::string& id);
    virtual ~Rule();

    const std::string& id() const;

    //virtual const std::string& name() const = 0;
    virtual void evaluate() const = 0;

private:
    const std::string _id;
};
