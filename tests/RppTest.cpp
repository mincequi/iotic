#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <QCoreApplication>
#include <QTimer>

#include <rpp/rpp.hpp>

#include <common/Logger.h>
#include <things/ThingsRepository.h>

class SomeThing : public Thing {
public:
    using Thing::Thing;
    virtual void doRead() override {};
    virtual void doSetProperty(MutableProperty property, const Value& value) override {};

    void tick() {
        static int i = 0;
        if (i%4 == 0)
            _propertiesSubject.get_subscriber().on_next({});
        if (i%4 == 1)
            _propertiesSubject.get_subscriber().on_next({});
        if (i%4 == 2)
            _propertiesSubject.get_subscriber().on_next({});
        if (i%4 == 3)
            _propertiesSubject.get_subscriber().on_completed();
        ++i;
    }
};

//TEST_CASE("Thing can be deleted", "[rpp]") {
int main() {
    int i = 0;
    QCoreApplication a(i, nullptr);

    ThingsRepository _repo;
    _repo.thingAdded().subscribe([](const auto& thing) {
        thing->properties().subscribe([](const auto& prop) {
        });
    });

    ThingInfo thingInfo(ThingInfo::DiscoveryType::Http, "id", "host");
    _repo.addThing(std::make_unique<SomeThing>(thingInfo));

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
