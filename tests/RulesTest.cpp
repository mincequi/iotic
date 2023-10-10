#include <catch2/catch_test_macros.hpp>

#include <exprtk.hpp>
#include <rpp/schedulers/immediate_scheduler.hpp>

#include <rules/GenericActuationStrategy.h>
#include <rules/test_scheduler.hpp>
#include "TestUtil.h"

TEST_CASE("OnOffRule", "[rules]") {
    TestUtil util;
    util.setOnOffRule("id", "id.temperature < 51", "id.temperature > 55");
    util.setProperty("id", Property::temperature, 52);
    util.onOffRule()->evaluate();

    const auto& thing = util.thingById("id");
    REQUIRE(thing->mutableProperties().contains(MutableProperty::power_control) == false);

    util.setProperty("id", Property::temperature, 50);
    util.onOffRule()->evaluate();
    REQUIRE(thing->mutableProperties().contains(MutableProperty::power_control) == true);
    REQUIRE(thing->mutableProperties().at(MutableProperty::power_control) == Value(true));

    util.setProperty("id", Property::temperature, 60);
    util.onOffRule()->evaluate();
    REQUIRE(thing->mutableProperties().at(MutableProperty::power_control) == Value(true));

    auto rule = (GenericActuationStrategy<test_scheduler>*)util.onOffRule();
    rule->scheduler().time_advance(std::chrono::seconds{2});
    util.onOffRule()->evaluate();
    REQUIRE(thing->mutableProperties().at(MutableProperty::power_control) == Value(false));
}
