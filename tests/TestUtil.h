#pragma once

#include <things/Thing.h>

class Strategy;

class TestUtil {
public:
    TestUtil();

    static void createThing(const std::string& id);
    static void setProperty(const std::string& id, Property property, const Value& value);

    static double symbol(const std::string& id);
};
