#include "TestUtil.h"

#include <common/Util.h>
#include <config/Config.h>
#include <rules/RuleActuationStrategy.h>
#include <rules/RulesEngine.h>
#include <rules/test_scheduler.hpp>
#include <things/ThingsRepository.h>

class TestThing : public Thing {
public:
    using Thing::Thing;
    virtual void doRead() override {};
    virtual void doSetProperty(MutableProperty property, const Value& value) override {};

    void setProperty(Property property, const Value& value) {
        _propertiesSubject.get_observer().on_next({{ property, value }});
    }
};

TestUtil::TestUtil() {
}

void TestUtil::createThing(const std::string& id) {
    const auto& thing = repo->thingById(id);
    if (!thing) {
        ThingInfo thingInfo(ThingInfo::DiscoveryType::Http, id, id);
        auto t = std::make_unique<TestThing>(thingInfo);
        repo->addThing(std::move(t));
    }
}

void TestUtil::setProperty(const std::string& id, Property property, const Value& value) {
    const auto& thing = repo->thingById(id);
    if (!thing) {
        ThingInfo thingInfo(ThingInfo::DiscoveryType::Http, id, id);
        auto t = std::make_unique<TestThing>(thingInfo);
        repo->addThing(std::move(t));
    }
    const auto testThing = (TestThing*)(repo->thingById(id).get());
    testThing->setProperty(property, value);
    const std::string var = id + "." + util::toString(property);
    rules->_symbolTable[var] = toDouble(value);
}

double TestUtil::symbol(const std::string& id) {
    return rules->_symbolTable[id];
}
