#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <QCoreApplication>
#include <QTimer>

#include <rpp/rpp.hpp>

#include <common/Logger.h>
#include <things/ThingsRepository.h>

#include "things/TestThing.h"

using uvw_iot::common::ThingPtr;

//TEST_CASE("Thing can be deleted", "[rpp]") {
int main() {
    int i = 0;
    QCoreApplication a(i, nullptr);

    repo->thingAdded().subscribe([](ThingPtr thing) {
        thing->properties().subscribe([](const auto& prop) {
        });
    });

    ThingInfo thingInfo(ThingInfo::DiscoveryType::Http, "id", "host");
    repo->addThing(std::make_unique<TestThing>(thingInfo));

    const auto& thing = _repo.thingById("id");
    SomeThing* someThing = static_cast<SomeThing*>(thing.get());

    QTimer timer;
    timer.callOnTimeout([&]() {
        someThing->tick();
        const auto s = _repo.things().size();
        std::cerr << "things count: " << s << std::endl;
    });
    timer.start(1000);

    return a.exec();
}
