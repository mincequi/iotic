#include <catch2/catch_test_macros.hpp>

class SomeClass {
};

TEST_CASE("unique_ptr_test", "[cpp]") {
    std::unique_ptr<SomeClass> somePtr;
    REQUIRE_FALSE(somePtr);

    somePtr = nullptr;
    REQUIRE_FALSE(somePtr);
}
