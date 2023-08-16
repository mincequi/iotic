#pragma once

#include <things/http/HttpThing.h>

class Shelly : public HttpThing {
public:
    Shelly(const ThingInfo& info, bool shallRead);

    static ThingPtr from(const ThingInfo& ThingInfo);

private:
    void setProperty(WriteableThingProperty property, double value) override;
    void doRead() override;
    void onRead(const QByteArray& response) override;

    bool _isPm = false;
};
