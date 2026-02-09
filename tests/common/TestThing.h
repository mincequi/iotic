#pragma once

#include <uvw_iot/Thing.h>

class TestThing : public uvw_iot::Thing {
public:
    TestThing(const std::string& id,
              uvw_iot::ThingType type = uvw_iot::ThingType::Unknown)
        : _id(id),
          _type(type) {
    }

    inline virtual const std::string& id() const override { return _id; }
    inline virtual uvw_iot::ThingType type() const override { return _type; }

private:
    std::string _id;
    uvw_iot::ThingType _type = uvw_iot::ThingType::Unknown;
};
