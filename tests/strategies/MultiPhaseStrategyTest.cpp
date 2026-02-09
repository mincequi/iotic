#include <catch2/catch_test_macros.hpp>

#include <common/TestThing.h>
#include <config/ConfigRepository.h>
#include <strategies/MultiPhaseStrategy.h>
#include <uvw_iot/ThingProperty.h>
#include <uvw_iot/ThingRepository.h>

using namespace uvw_iot;

TEST_CASE("MultiPhaseStrategy runs with different configurations", "[MultiPhaseStrategy]") {
    std::string id = "shellypro3_ece334f7be9c";
    auto thing = std::make_shared<TestThing>(TestThing(id, ThingType::Relay));

    ThingPropertyMap properties;
    properties.set<ThingPropertyKey::digitalInput>(std::vector<bool>{true, true, true});
    properties.set<ThingPropertyKey::multistateSelector>(std::vector<bool>{false, false, false});
    thing->setProperties(properties);

    uvw_iot::ThingRepository thingRepository;
    ConfigRepository configRepository(thingRepository);

    auto multiPhaseStrategy = MultiPhaseStrategy::from(thing, configRepository);
    REQUIRE(multiPhaseStrategy);
    MultiPhaseStrategy* cut = (MultiPhaseStrategy*)multiPhaseStrategy.get();

    SECTION("Phase up all three phases") {
        Site::Properties siteProperties{
            .ts = 1800,
            .longTermGridPower = -2751
        };
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);
        REQUIRE(!cut->wantsToStepUp(siteProperties));
        REQUIRE(!cut->wantsToStepDown(siteProperties));

        // Now inputs get false
        properties.set<ThingPropertyKey::digitalInput>(std::vector<bool>{false, false, false});
        properties.set<ThingPropertyKey::multistateSelector>(std::vector<bool>{true, true, true});
        thing->setProperties(properties);
        REQUIRE(cut->wantsToStepDown(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Down, siteProperties);
        REQUIRE(cut->wantsToStepDown(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Down, siteProperties);
        REQUIRE(cut->wantsToStepDown(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Down, siteProperties);
        REQUIRE(!cut->wantsToStepUp(siteProperties));
        REQUIRE(!cut->wantsToStepDown(siteProperties));

        // Now inputs get true again
        properties.set<ThingPropertyKey::digitalInput>(std::vector<bool>{true, true, true});
        properties.set<ThingPropertyKey::multistateSelector>(std::vector<bool>{false, false, false});
        thing->setProperties(properties);
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);

        // Only after 30 minutes we can step up again
        REQUIRE(!cut->wantsToStepUp(siteProperties));
        siteProperties.ts += 1800;
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);

        // Now inputs get false
        properties.set<ThingPropertyKey::digitalInput>(std::vector<bool>{false, false, false});
        properties.set<ThingPropertyKey::multistateSelector>(std::vector<bool>{true, true, true});
        thing->setProperties(properties);
        REQUIRE(cut->wantsToStepDown(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Down, siteProperties);
        siteProperties.ts += 1800;
        REQUIRE(cut->wantsToStepDown(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Down, siteProperties);
        REQUIRE(cut->wantsToStepDown(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Down, siteProperties);
        REQUIRE(!cut->wantsToStepUp(siteProperties));
        REQUIRE(!cut->wantsToStepDown(siteProperties));

        // Now inputs get true again
        properties.set<ThingPropertyKey::digitalInput>(std::vector<bool>{true, true, true});
        properties.set<ThingPropertyKey::multistateSelector>(std::vector<bool>{false, false, false});
        thing->setProperties(properties);
        REQUIRE(cut->wantsToStepUp(siteProperties));
        cut->adjust(MultiPhaseStrategy::Step::Up, siteProperties);
        REQUIRE(!cut->wantsToStepUp(siteProperties));
    }
}
