#include "TestUtil.h"

#include <common/Util.h>
#include <rules/GenericActuationStrategy.h>
#include <rules/test_scheduler.hpp>

class TestThing : public Thing {
public:
    using Thing::Thing;
    virtual void doRead() override {};
    virtual void doSetProperty(MutableProperty property, const Value& value) override {};

    void setProperty(Property property, const Value& value) {
        _propertiesSubject.get_subscriber().on_next({{ property, value }});
    }
};

TestUtil::TestUtil() {
    _parser.enable_unknown_symbol_resolver();
}

void TestUtil::setProperty(const std::string& id, Property property, const Value& value) {
    const auto& thing = _repo.thingById(id);
    if (!thing) {
        ThingInfo thingInfo(ThingInfo::DiscoveryType::Http, id, id);
        auto t = std::make_unique<TestThing>(thingInfo);
        _repo.addThing(std::move(t));
    }
    const auto testThing = (TestThing*)(_repo.thingById(id).get());
    testThing->setProperty(property, value);
    const std::string var = id + "." + util::toString(property);
    _symbolTable.variable_ref(var) = toDouble(value);
}

void TestUtil::setOnOffRule(const std::string& id,
                            const std::string& onExpressionStr,
                            const std::string& offExpressionStr) {

    auto onExpression = std::make_unique<exprtk::expression<double>>();
    onExpression->register_symbol_table(_symbolTable);
    if (!_parser.compile(onExpressionStr, *onExpression)) {
        return;
    }

    auto offExpression = std::make_unique<exprtk::expression<double>>();
    offExpression->register_symbol_table(_symbolTable);
    if (!_parser.compile(offExpressionStr, *offExpression)) {
        return;
    }

    _onOffRule = std::make_unique<GenericActuationStrategy<test_scheduler>>(id, std::move(onExpression), std::move(offExpression), [&](bool isOn) {
        _repo.setThingProperty(id, MutableProperty::power_control, isOn);
    });
}

const ThingPtr& TestUtil::thingById(const std::string& id) const {
    return _repo.thingById(id);
}

Strategy* TestUtil::onOffRule() const {
    return _onOffRule.get();
}
