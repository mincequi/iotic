#pragma once

#include <things/Thing.h>

class HttpThing : public Thing {
public:
    explicit HttpThing(const ThingInfo& info);
    virtual ~HttpThing();

protected:
    struct Impl;
    std::unique_ptr<Impl> _p;

    void read(const std::string& host, const std::string& path);
    void write(const std::string& host, const std::string& path);

private:
    // TODO: this does not work, because thing might be destroyed when answer returns.
    void onHttpRead(const std::string& response, int error);

    uint8_t _errorCount = 0;
};
