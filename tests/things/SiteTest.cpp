#include <catch2/catch_test_macros.hpp>

#include <rpp/observables/dynamic_observable.hpp>

#include <things/Site.h>
#include <things/ThingsRepository.h>

#include "TestThing.h"

TEST_CASE("site_test", "[cpp]") {
    std::vector<int> result;
    _site->gridPower().subscribe([&](int power) {
        result.push_back(power);
    });

    ThingInfo thingInfo(ThingInfo::DiscoveryType::Http, "id", "host");
    auto t = std::make_unique<TestThing>(thingInfo);
    repo->addThing(std::move(t));

    TestThing* thing = static_cast<TestThing*>(repo->thingById("id").get());
    thing->setProperty(Property::power, 5);
    thing->setProperty(Property::power, 7);
    thing->setProperty(Property::power, 9);
    thing->setProperty(Property::power, 1);
    thing->setProperty(Property::power, 8);
    thing->setProperty(Property::power, 3);
    thing->setProperty(Property::power, 7);
    thing->setProperty(Property::power, 2);
    thing->setProperty(Property::power, 6);
    thing->setProperty(Property::power, 1);

    REQUIRE(result == std::vector<int>{ 5, 5, 7, 7, 8, 3, 7, 3, 6, 2 });
}
