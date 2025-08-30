#include <catch2/catch_test_macros.hpp>

#include <tinyexpr.h>
#include <rpp/schedulers/immediate_scheduler.hpp>

#include <common/Logger.h>
#include <config/ConfigRepository.h>
#include <rules/RuleActuationStrategy.h>
#include <rules/RulesEngine.h>
#include <rules/test_scheduler.hpp>
#include <things/ThingsRepository.h>
#include "TestUtil.h"

//const std::string onStr("and(id.temperature < 53, grid_power < -1400 + id.offset)");
//const std::string offStr("or(id.temperature > 55, grid_power > id.offset)");
const std::string onStr("id.temperature < 53 & grid_power < -1400 + id.offset");
const std::string offStr("id.temperature > 55 | grid_power > id.offset");

TEST_CASE("TinyexprTest", "[rules]") {
    std::map<std::string, double> symbolTable;
    te_parser evalParser;
    evalParser.set_unknown_symbol_resolver([&](std::string_view symbol) {
        LOG_S(INFO) << "resolving symbol: " << symbol;
        return symbolTable[std::string(symbol)];
    });
    REQUIRE(evalParser.compile(onStr));
    REQUIRE(evalParser.compile(offStr));

    te_parser onParser;
    te_parser offParser;
    for (const auto& kv : symbolTable) {
        auto var = std::string(kv.first);
        LOG_S(INFO) << "add variable: " << var;
        onParser.add_variable_or_function({ var, &kv.second });
        offParser.add_variable_or_function({ var, &kv.second });
    }
    REQUIRE(onParser.compile(onStr));
    REQUIRE(offParser.compile(offStr));

    REQUIRE(!onParser.evaluate());
    REQUIRE(!offParser.evaluate());

    symbolTable["id.offset"] = 1401;
    REQUIRE(onParser.evaluate());
    symbolTable["grid_power"] = 1;
    REQUIRE(!onParser.evaluate());
    symbolTable["grid_power"] = 0;
    symbolTable["id.temperature"] = 56;
    REQUIRE(!onParser.evaluate());
    REQUIRE(offParser.evaluate());

    symbolTable["id.temperature"] = 55;
    REQUIRE(!offParser.evaluate());
    symbolTable["grid_power"] = 1402;
    REQUIRE(offParser.evaluate());
    symbolTable["grid_power"] = 1401;
    REQUIRE(!offParser.evaluate());
}

TEST_CASE("TinyexprTest2", "[rules]") {
    std::map<std::string, double> symbolTable;
    te_parser evalParser;
    evalParser.set_unknown_symbol_resolver([&](std::string_view symbol) {
        LOG_S(INFO) << "resolving symbol: " << symbol;
        return symbolTable[std::string(symbol)];
    }, false);
    REQUIRE(evalParser.compile(onStr));

    for (const auto& kv : symbolTable) {
        auto var = std::string(kv.first);
        LOG_S(INFO) << "add variable: " << var;
        evalParser.add_variable_or_function({ var, &kv.second });
    }
    REQUIRE(evalParser.compile(onStr));
    REQUIRE(!evalParser.evaluate());
    symbolTable["id.offset"] = 1401;
    REQUIRE(evalParser.evaluate());
}

TEST_CASE("RulesEngineTest", "[rules]") {
    auto onParser = rules->createParser(onStr);
    auto offParser = rules->createParser(offStr);
    REQUIRE(onParser->get_variables_and_functions().size() == 3);
    REQUIRE(offParser->get_variables_and_functions().size() == 3);
    REQUIRE(rules->resolveSymbol("grid_power") == 0.0);
    REQUIRE(rules->resolveSymbol("id.temperature") == 0.0);
    REQUIRE(rules->resolveSymbol("id.offset") == 0.0);

    TestUtil::setProperty("id", Property::temperature, 1);
    TestUtil::setProperty("id", Property::offset, 140000000);
    REQUIRE(rules->resolveSymbol("id.temperature") == 1.0);
    REQUIRE(rules->resolveSymbol("id.offset") == 140000000.0);
}

TEST_CASE("RuleActuationStrategyTest", "[rules]") {
    // Pre-requirements
    Config::init(*repo);
    auto r = rules;
    TestUtil::createThing("id2");

    // No on/off rule provided, so no strategy created
    //auto cut = RuleActuationStrategy<test_scheduler>::from(repo->thingById("id2"));
    //REQUIRE(!cut);

    // Provide on/off rule, so strategy gets created
    cfg->_testing = true;
    cfg->setValue("id2", Property::on, onStr);
    cfg->setValue("id2", Property::off, offStr);
    auto cut = RuleActuationStrategy<test_scheduler>::from(repo->thingById("id2"));
    REQUIRE(cut);

    // Although we are using sections here, the repo is not being cleaned.
    // This has to be considered when writing tests here.
    SECTION("id2 gets powered on") {
        REQUIRE(!repo->thingById("id2")->mutableProperties().contains(MutableProperty::power_control));
        TestUtil::setProperty("id", Property::temperature, 52);
        TestUtil::setProperty("id", Property::offset, 1401);
        REQUIRE(TestUtil::symbol("id.temperature") == 52.0);
        REQUIRE(TestUtil::symbol("id.offset") == 1401.0);
        //repo->setThingProperty("id", MutableProperty::offset, 1401);
        cut->evaluate();
        REQUIRE(repo->thingById("id2")->mutableProperties().at(MutableProperty::power_control) == Value(true));
    }

    SECTION("id2 is still on") {
        TestUtil::setProperty("id", Property::temperature, 55);
        cut->evaluate();
        REQUIRE(repo->thingById("id2")->mutableProperties().at(MutableProperty::power_control) == Value(true));
    }

    SECTION("id2 gets powered off") {
        TestUtil::setProperty("id", Property::temperature, 56);
        cut->evaluate();
        REQUIRE(repo->thingById("id2")->mutableProperties().at(MutableProperty::power_control) == Value(false));
    }
}
