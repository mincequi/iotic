#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>
#include <uvw_iot/common/ThingRepository.h>

#include <config/Config.h>
#include <webserver/WebAppRouter.h>

using json = nlohmann::json;

class WebAppRouterTest {
public:
    WebAppRouterTest() :
        cfg(thingRepository),
        site(thingRepository, cfg),
        router(thingRepository, site, cfg) {
    }

protected:
    std::vector<uint8_t> serializeSiteHistory(const std::list<Site::SiteData>& history, uint from, uint to) {
        return router.serializeSiteHistory(history, from, to);
    }

private:
    ThingRepository thingRepository;
    Config cfg;
    Site site;
    WebAppRouter router;
};

TEST_CASE_METHOD(WebAppRouterTest, "Serialize Site History", "[serialize]") {
    std::list<Site::SiteData> history;

    history.push_back(Site::SiteData {20, 10, 10} );
    history.push_back(Site::SiteData {30, 10, 10} );
    history.push_back(Site::SiteData {40, 10, 10} );

    // We shall leave out anything greater or equal than upper limit
    REQUIRE(serializeSiteHistory(history, 0, 19).empty());
    REQUIRE(serializeSiteHistory(history, 0, 20).empty());
    REQUIRE(json::from_cbor(serializeSiteHistory(history, 0, 21))["site"]["timestamp"] == std::vector<uint>{20});

    REQUIRE(json::from_cbor(serializeSiteHistory(history, 20, 41))["site"]["timestamp"] == std::vector<uint>{20,10,10});
    REQUIRE(json::from_cbor(serializeSiteHistory(history, 20, 40))["site"]["timestamp"] == std::vector<uint>{20,10});

    // We shall include one item lower than the lower limit
    REQUIRE(json::from_cbor(serializeSiteHistory(history, 29, 40))["site"]["timestamp"] == std::vector<uint>{20,10});

    REQUIRE(json::from_cbor(serializeSiteHistory(history, 40, 41))["site"]["timestamp"] == std::vector<uint>{40});
    REQUIRE(serializeSiteHistory(history, 40, 40).empty());
    REQUIRE(json::from_cbor(serializeSiteHistory(history, 30, 31))["site"]["timestamp"] == std::vector<uint>{30});

    REQUIRE(serializeSiteHistory(history, 30, 30).empty());

    // If we exceed our history only the last element of our history shall be sent
    REQUIRE(json::from_cbor(serializeSiteHistory(history, 50, 60))["site"]["timestamp"] == std::vector<uint>{40});
}
