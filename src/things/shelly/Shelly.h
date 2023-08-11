#pragma once

#include <things/http/HttpThing.h>

class Shelly : public HttpThing {
public:
    Shelly(const ThingInfo& info, bool shallRead);

    static ThingPtr from(const ThingInfo& ThingInfo);

    void doRead() override;
    void onRead(const QByteArray& response) override;

private:
    bool _shallRead = false;
};
