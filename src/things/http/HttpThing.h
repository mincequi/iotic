#pragma once

#include <things/Thing.h>

class HttpThing : public Thing {
public:
    explicit HttpThing(const ThingInfo& info);
    virtual ~HttpThing();

protected:
    struct Impl;
    std::unique_ptr<Impl> _p;

    void read(const std::string& url);
    void write(const std::string& url);
};
