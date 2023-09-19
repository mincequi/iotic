#pragma once

#include <things/http/HttpThing.h>

class Shelly : public HttpThing {
public:
    static ThingPtr from(const ThingInfo& info);

private:
    Shelly(const ThingInfo& info, bool shallRead);
    void doSetProperty(MutableProperty property, const Value& value) override;
    void doRead() override;
    void onRead(const QByteArray& response) override;

    bool _isPm = false;
};
